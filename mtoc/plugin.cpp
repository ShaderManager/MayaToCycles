#include <maya/MFnPlugin.h>

#include "render_cmd.hpp"

MStatus initializePlugin(MObject plugin_object)
{
	MStatus status;
	MFnPlugin plugin(plugin_object, "_ShaMan_", "0.1", "Any", &status);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerCommand(RenderCmd::name, RenderCmd::creator, RenderCmd::cmd_syntax);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin_object)
{
	MFnPlugin plugin(plugin_object);
	MStatus status;

	status = plugin.deregisterCommand(RenderCmd::name);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}
