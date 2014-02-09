#include "session.h"
#include "scene.h"
#include "buffers.h"

#include <maya/MRenderView.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MItDag.h>

#include "render_cmd.hpp"

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
			if (it_dag.fullPathName() == camera_name)
			{
				MDagPath::getAPathTo(it_dag.currentItem(), camDagPath);
			}
		}
	}

	MRenderView::setCurrentCamera(camDagPath);

	ccl::SessionParams session_params;
	ccl::SceneParams scene_params;

	session_params.background = true;

	m_session = new ccl::Session(session_params);
	m_scene = new ccl::Scene(scene_params);

	int samples = 1;

	m_session->scene = m_scene;
	m_session->progress.set_update_callback(function_bind(&RenderCmd::update_framebuffer, this));
	m_session->reset(m_width, m_height, samples);

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
