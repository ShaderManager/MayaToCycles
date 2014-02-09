#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "device.h"
#include "../cycles/render/filter.h"

#include "render_globals_node.hpp"

MString RenderGlobalsNode::name("cyclesRenderGlobals");
MTypeId RenderGlobalsNode::type_id(0x65FA12, 1);

MObject RenderGlobalsNode::numRenderSamplesAttr;
MObject RenderGlobalsNode::computeDeviceAttr;

MObject RenderGlobalsNode::filterTypeAttr;
MObject RenderGlobalsNode::filterWidthAttr;

MStatus RenderGlobalsNode::initializer()
{
	MStatus status;
	MFnNumericAttribute num_attr;
	MFnEnumAttribute enum_attr;

	numRenderSamplesAttr = num_attr.create("numRenderSamples", "smp", MFnNumericData::kInt, 10, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	computeDeviceAttr = enum_attr.create("computeDevice", "dev", ccl::DEVICE_CPU, &status);
	enum_attr.addField("CPU", ccl::DEVICE_CPU);
	enum_attr.addField("OpenCL", ccl::DEVICE_OPENCL);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	filterTypeAttr = enum_attr.create("filterType", "flt", 1, &status);
	enum_attr.addField("Box", ccl::FILTER_BOX);
	enum_attr.addField("Gaussian", ccl::FILTER_GAUSSIAN);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	filterWidthAttr = num_attr.create("filterWidth", "flw", MFnNumericData::kFloat, 1.0, &status);
	num_attr.setMin(0.01);
	num_attr.setMax(10.0);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(numRenderSamplesAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(computeDeviceAttr));

	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(filterTypeAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(filterWidthAttr));

	return MStatus::kSuccess;
}
