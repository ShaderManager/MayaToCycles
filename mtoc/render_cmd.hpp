#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class MDagPath;

namespace ccl
{
	class Session;
	class Scene;
	struct RenderTile;
}

class RenderCmd : public MPxCommand
{
public:
	static MString name;

	static void* creator()
	{
		return new RenderCmd();
	}

	static MSyntax cmd_syntax();

private:
	RenderCmd();

	MStatus	doIt(const MArgList& args);

	ccl::Session* m_session;
	ccl::Scene* m_scene;
	unsigned int m_width, m_height;

	void update_tile(ccl::RenderTile& tile);

	void sync_camera(const MDagPath& camera_path);
	void sync_meshes();
};
