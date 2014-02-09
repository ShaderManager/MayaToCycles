#pragma once

#include <maya/MPxNode.h>

class RenderGlobalsNode : public MPxNode
{
public:
	static MString name;
	static MTypeId type_id;

	static void* creator()
	{
		return new RenderGlobalsNode();
	}
	
	static MStatus initializer();

	// Attributes
	static MObject numRenderSamplesAttr;
	static MObject computeDeviceAttr;

	// Integrator attributes
	static MObject seedAttr;
	static MObject minBounceAttr;
	static MObject maxBounceAttr;

	static MObject maxDiffuseBounceAttr;
	static MObject maxGlossyBounceAttr;
	static MObject maxTransmissionBounceAttr;

	static MObject probalisticTerminationAttr;

	static MObject transparentMinBounceAttr;
	static MObject transparentMaxBounceAttr;
	static MObject transparentProbalisticAttr;
	static MObject transparentShadowsAttr;

	static MObject causticsAttr;

private:
	RenderGlobalsNode() {}
};
