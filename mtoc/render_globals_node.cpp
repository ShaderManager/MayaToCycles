#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "device.h"

#include "render_globals_node.hpp"

MString RenderGlobalsNode::name("cyclesRenderGlobals");
MTypeId RenderGlobalsNode::type_id(0x65FA12, 1);

MObject RenderGlobalsNode::numRenderSamplesAttr;
MObject RenderGlobalsNode::computeDeviceAttr;

// Integrator attributes
MObject RenderGlobalsNode::seedAttr;
MObject RenderGlobalsNode::minBounceAttr;
MObject RenderGlobalsNode::maxBounceAttr;
MObject RenderGlobalsNode::maxDiffuseBounceAttr;
MObject RenderGlobalsNode::maxGlossyBounceAttr;
MObject RenderGlobalsNode::maxTransmissionBounceAttr;
MObject RenderGlobalsNode::probalisticTerminationAttr;
MObject RenderGlobalsNode::transparentMinBounceAttr;
MObject RenderGlobalsNode::transparentMaxBounceAttr;
MObject RenderGlobalsNode::transparentProbalisticAttr;
MObject RenderGlobalsNode::transparentShadowsAttr;
MObject RenderGlobalsNode::causticsAttr;

MStatus RenderGlobalsNode::initializer()
{
	MStatus status;
	MFnNumericAttribute num_attr;
	MFnEnumAttribute enum_attr;

	numRenderSamplesAttr = num_attr.create("numRenderSamples", "smp", MFnNumericData::kInt, 10, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	computeDeviceAttr = enum_attr.create("computeDevice", "dev", ccl::DEVICE_CPU, &status);
	for each(ccl::DeviceType dt in ccl::Device::available_types())
	{
		switch (dt)
		{
		case ccl::DEVICE_NONE:
			enum_attr.addField("None", ccl::DEVICE_NONE);
			break;
		case ccl::DEVICE_CPU:
			enum_attr.addField("CPU", ccl::DEVICE_CPU);
			break;
		case ccl::DEVICE_OPENCL:
			enum_attr.addField("OpenCL", ccl::DEVICE_OPENCL);
			break;
		case ccl::DEVICE_CUDA:
			enum_attr.addField("CUDA", ccl::DEVICE_CUDA);
			break;
		case ccl::DEVICE_NETWORK:
			enum_attr.addField("Network", ccl::DEVICE_NETWORK);
			break;
		case ccl::DEVICE_MULTI:
			enum_attr.addField("Multi", ccl::DEVICE_MULTI);
			break;
		}
	}

	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(numRenderSamplesAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(computeDeviceAttr));

	// Integrator attributes
	seedAttr = num_attr.create("seed", "sd", MFnNumericData::kInt, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	minBounceAttr = num_attr.create("minBounce", "mnb", MFnNumericData::kInt, 2, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	maxBounceAttr = num_attr.create("maxBounce", "mxb", MFnNumericData::kInt, 7, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	maxDiffuseBounceAttr = num_attr.create("maxDiffuseBounce", "mxdb", MFnNumericData::kInt, 7, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	maxGlossyBounceAttr = num_attr.create("maxGlossyBounce", "mxgb", MFnNumericData::kInt, 7, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	maxTransmissionBounceAttr = num_attr.create("maxTransmissionBounce", "mxtb", MFnNumericData::kInt, 7, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	transparentMinBounceAttr = num_attr.create("transparentMinBounceAttr", "tmnb", MFnNumericData::kInt, 2, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	transparentMaxBounceAttr = num_attr.create("transparentMaxBounceAttr", "tmxb", MFnNumericData::kInt, 7, &status);
	num_attr.setMin(1);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	probalisticTerminationAttr = num_attr.create("probalisticTermination", "pbtr", MFnNumericData::kBoolean, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	transparentProbalisticAttr = num_attr.create("transparentProbalistic", "tptr", MFnNumericData::kBoolean, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	transparentShadowsAttr = num_attr.create("transparentShadows", "trsd", MFnNumericData::kBoolean, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	causticsAttr = num_attr.create("caustics", "cstc", MFnNumericData::kBoolean, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(seedAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(minBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(maxBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(maxDiffuseBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(maxGlossyBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(maxTransmissionBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(probalisticTerminationAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(transparentMinBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(transparentMaxBounceAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(transparentProbalisticAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(transparentShadowsAttr));
	CHECK_MSTATUS_AND_RETURN_IT(addAttribute(causticsAttr));

	return MStatus::kSuccess;
}
