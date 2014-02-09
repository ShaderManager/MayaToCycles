#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "util_path.h"

#include "render_cmd.hpp"
#include "render_globals_node.hpp"

MStatus initializePlugin(MObject plugin_object)
{
	MStatus status;
	MFnPlugin plugin(plugin_object, "_ShaMan_", "0.2", "Any", &status);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerCommand(RenderCmd::name, RenderCmd::creator, RenderCmd::cmd_syntax);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode(RenderGlobalsNode::name, RenderGlobalsNode::type_id, RenderGlobalsNode::creator, RenderGlobalsNode::initializer);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	MString init_python_package = "import sys; sys.path.append(\"";
	init_python_package += "d:/Coding/C++/Projects/MayaCycles/scripts/";
	init_python_package += "\")";

	ccl::path_init(plugin.loadPath().asChar());

	status = MGlobal::executePythonCommand(init_python_package);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = MGlobal::executeCommand("global proc cyclesStartup() { python(\"from mtoc.cyclesStartup import cyclesStartup; cyclesStartup()\"); }");
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = MGlobal::executeCommand("global proc cyclesShutdown() { python(\"from mtoc.cyclesStartup import cyclesShutdown; cyclesShutdown()\"); }");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerUI("cyclesStartup", "cyclesShutdown");

	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin_object)
{
	MFnPlugin plugin(plugin_object);
	MStatus status;

	status = plugin.deregisterCommand(RenderCmd::name);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(RenderGlobalsNode::type_id);

	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}
