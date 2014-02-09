import maya.cmds as cmds
import pymel.core as core

def createDefault(defaultNodeName):
	core.createNode('cyclesRenderGlobals', skipSelect = True, shared = True, name = defaultNodeName) 

def deleteDefault(defaultNodeName):
	pass
	#core.delete(defaultNodeName)