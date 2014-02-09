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

	// Filter attributes
	static MObject filterTypeAttr;
	static MObject filterWidthAttr;

private:
	RenderGlobalsNode() {}
};
