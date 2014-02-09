#include "session.h"
#include "scene.h"
#include "buffers.h"
#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "../cycles/render/filter.h"

#include <maya/MRenderView.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MItDag.h>
#include <maya/MFnCamera.h>
#include <maya/MMatrix.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnDependencyNode.h>

#include "render_globals_node.hpp"

#include "render_cmd.hpp"

static inline ccl::Transform convert_MMatrix_to_Transform(const MMatrix& transform)
{
	ccl::Transform result;

	result.x.x = transform[0][0]; result.x.y = transform[1][0]; result.x.z = transform[2][0]; result.x.w = transform[3][0];
	result.y.x = transform[0][1]; result.y.y = transform[1][1]; result.y.z = transform[2][1]; result.y.w = transform[3][1];
	result.z.x = transform[0][2]; result.z.y = transform[1][2]; result.z.z = transform[2][2]; result.z.w = transform[3][2];
	result.w.x = transform[0][3]; result.w.y = transform[1][3]; result.w.z = transform[2][3]; result.w.w = transform[3][3];

	return result;
}

MString RenderCmd::name("cycles_render");

MSyntax RenderCmd::cmd_syntax()
{
	MSyntax syntax;

	syntax.addFlag("-w", "-width", MSyntax::kUnsigned);
	syntax.addFlag("-h", "-height", MSyntax::kUnsigned);
	syntax.addFlag("-cam", "-camera", MSyntax::kSelectionItem);

	return syntax;
}

RenderCmd::RenderCmd() : m_width(640), m_height(480)
{

}

MStatus RenderCmd::doIt(const MArgList& args)
{
	MArgDatabase arg_db(syntax(), args);

	if (arg_db.isFlagSet("-w"))
	{
		m_width = arg_db.flagArgumentInt("-w", 0);
	}

	if (arg_db.isFlagSet("-h"))
	{
		m_height = arg_db.flagArgumentInt("-h", 0);
	}

	if (!MRenderView::doesRenderEditorExist())
	{
		displayError("Render View is not opened at current moment");
		return MStatus::kFailure;
	}
	
	MDagPath camDagPath;

	if (!arg_db.isFlagSet("-cam"))
	{
		M3dView curView = M3dView::active3dView();
		curView.getCamera(camDagPath);
	}
	else
	{
		MString camera_name = arg_db.flagArgumentString("-cam", 0);
		for (MItDag it_dag; !it_dag.isDone(); it_dag.next())
		{
			if (it_dag.partialPathName() == camera_name)
			{
				MDagPath::getAPathTo(it_dag.currentItem(), camDagPath);
				break;
			}
		}

		if (!camDagPath.isValid())
		{
			M3dView curView = M3dView::active3dView();
			curView.getCamera(camDagPath);
		}
	}	

	MRenderView::setCurrentCamera(camDagPath);

	MFnDependencyNode render_globals_node;

	for (MItDependencyNodes it_dg; !it_dg.isDone(); it_dg.next())
	{
		MFnDependencyNode dep_node(it_dg.thisNode());

		if (RenderGlobalsNode::name == dep_node.typeName())
		{
			render_globals_node.setObject(it_dg.thisNode());
			break;
		}
	}

	if (render_globals_node.object().isNull())
	{
		displayError("Scene doesn't have any cyclesRenderGlobals node");
		return MStatus::kFailure;
	}

	ccl::SessionParams session_params;
	ccl::SceneParams scene_params;

	session_params.device_type = (ccl::DeviceType)render_globals_node.findPlug(RenderGlobalsNode::computeDeviceAttr).asShort();
	session_params.background = true;

	m_session = new ccl::Session(session_params);
	m_scene = new ccl::Scene(scene_params);

	int samples = render_globals_node.findPlug(RenderGlobalsNode::numRenderSamplesAttr).asInt();

	m_session->scene = m_scene;
	//m_session->progress.set_update_callback(function_bind(&RenderCmd::update_framebuffer, this));
	m_session->reset(m_width, m_height, samples);

	m_scene->filter->filter_type = (ccl::FilterType)render_globals_node.findPlug(RenderGlobalsNode::filterTypeAttr).asShort();
	m_scene->filter->filter_width = render_globals_node.findPlug(RenderGlobalsNode::filterWidthAttr).asFloat();

	sync_camera(camDagPath);
	sync_meshes();

	MRenderView::startRender(m_width, m_height, true);

	m_session->start();
	m_session->wait();

	update_framebuffer();

	MRenderView::endRender();

	delete m_session;

	return MStatus::kSuccess;
}

void RenderCmd::update_framebuffer()
{
	int sample;
	double total_time, sample_time;
	m_session->progress.get_sample(sample, total_time, sample_time);

	ccl::float4* data = m_session->buffers->copy_from_device(1.0f, sample);

	if (!data)
		return;

	unsigned int left, right, bottom, top;
	//MRenderView::getRenderRegion(left, right, bottom, top);
	left = bottom = 0;
	right = m_width - 1;
	top = m_height - 1;

	MStatus status = MRenderView::updatePixels(left, right, bottom, top, (RV_PIXEL*)data, true);
	status = MRenderView::refresh(left, right, bottom, top);

	delete [] data;
}

void RenderCmd::sync_camera(const MDagPath& camera_path)
{
	MDagPath camera_shape_path(camera_path);
	camera_shape_path.extendToShapeDirectlyBelow(0);
	MFnCamera camera(camera_shape_path.node());

	float aspect = (float)m_width / m_height;
	float shiftX = camera.filmTranslateH();
	float shiftY = camera.filmTranslateV();

	float invAspect = 1.f / aspect;

	if (aspect > 1.f)
	{
		m_scene->camera->left = (2.f * shiftX) - 1;
		m_scene->camera->right = (2.f * shiftX) + 1;
		m_scene->camera->bottom = (2.f * shiftY) - invAspect;
		m_scene->camera->top = (2.f * shiftY) + invAspect;
	}
	else
	{
		m_scene->camera->left = (2.f * shiftX) - aspect;
		m_scene->camera->right = (2.f * shiftX) + aspect;
		m_scene->camera->bottom = (2.f * shiftY) - 1;
		m_scene->camera->top = (2.f * shiftY) + 1;
	}

	m_scene->camera->nearclip = camera.nearClippingPlane();
	m_scene->camera->farclip = camera.farClippingPlane();

	m_scene->camera->ortho = false;
	m_scene->camera->fov = (m_width > m_height) ? camera.horizontalFieldOfView() : camera.verticalFieldOfView();

	MMatrix transform = camera_shape_path.inclusiveMatrix().transpose();

	m_scene->camera->matrix = convert_MMatrix_to_Transform(camera_shape_path.inclusiveMatrix()) * ccl::transform_scale(1.0f, 1.0f, -1.0f);

	m_scene->camera->tag_update();
}

void RenderCmd::sync_meshes()
{
	for (MItDag it_dag; !it_dag.isDone(); it_dag.next())
	{
		if (it_dag.currentItem().hasFn(MFn::kMesh))
		{
			MFnMesh mesh(it_dag.currentItem());
			MDagPath transform_path;
			it_dag.getPath(transform_path);

			ccl::Mesh* cycles_mesh = new ccl::Mesh;
			
			MIntArray triangle_count, triangle_ids;
			MPointArray vertices;
			mesh.getTriangles(triangle_count, triangle_ids);
			mesh.getPoints(vertices);

			unsigned int num_faces = 0;
			for (unsigned int i = 0; i < triangle_count.length(); i++)
			{
				num_faces += triangle_count[i];
			}

			cycles_mesh->reserve(vertices.length(), num_faces);

			for (unsigned int i = 0; i < vertices.length(); i++)
			{
				MPoint p = vertices[i];
				cycles_mesh->verts[i] = ccl::make_float3(vertices[i].x, vertices[i].y, vertices[i].z);
			}

			for (unsigned int i = 0, face = 0; i < triangle_count.length(); i++)
			{
				for (unsigned int j = 0; j < triangle_count[i]; j++, face++)
				{
					cycles_mesh->triangles[face].v[0] = triangle_ids[face * 3 + 0];
					cycles_mesh->triangles[face].v[1] = triangle_ids[face * 3 + 1];
					cycles_mesh->triangles[face].v[2] = triangle_ids[face * 3 + 2];
					cycles_mesh->shader[face] = 0;
					cycles_mesh->smooth[face] = true;
				}
			}

			cycles_mesh->name = mesh.fullPathName().asChar();

			cycles_mesh->compute_bounds();
			cycles_mesh->tag_update(m_scene, true);

			m_scene->meshes.push_back(cycles_mesh);

			ccl::Object* node = new ccl::Object;
			node->mesh = cycles_mesh;
			node->name = it_dag.fullPathName().asChar();
			node->tfm = convert_MMatrix_to_Transform(transform_path.inclusiveMatrix());
			node->compute_bounds();
			node->tag_update(m_scene);

			m_scene->objects.push_back(node);
		}
	}
}
