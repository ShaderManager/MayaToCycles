import pymel.core as core
import maya.cmds as cmds
import maya.mel as mel

import cyclesOptions

def registerRenderer():
	mel.eval("""
		global proc cycles_render_callback(int $width, int $height, 
										int $doShadows,
										int $doGlowPass,
										string $camera, 
										string $option)
		{
			python("import mtoc.cyclesOptions;mtoc.cyclesOptions.createDefault('defaultCyclesOptions')");
			cycles_render -w $width -h $height -cam $camera;
		}
		""")

	core.renderer('cycles_renderer', rendererUIName = 'Cycles', renderProcedure = 'cycles_render_callback',
		addGlobalsTab = ('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"),
		addGlobalsNode = 'defaultCyclesOptions')

def unregisterRenderer():
	cyclesOptions.deleteDefault('defaultCyclesOptions')
	core.renderer('cycles_renderer', unr = True)
