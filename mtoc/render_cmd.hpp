#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

namespace ccl
{
	class Session;
	class Scene;
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

	void update_framebuffer();
};
