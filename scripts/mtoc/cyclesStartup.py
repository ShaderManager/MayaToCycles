import maya.mel as mel
import maya.cmds as cmds

import registerCyclesRenderer

def cyclesStartup():
	registerCyclesRenderer.registerRenderer()

def cyclesShutdown():
	registerCyclesRenderer.unregisterRenderer()