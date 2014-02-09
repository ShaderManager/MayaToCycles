include_directories( ${MAYA_INCLUDE_DIR})
include_directories( ../cycles/render ../cycles/device ../cycles/kernel ../cycles/kernel/svm ../cycles/util)

set(sources plugin.cpp
						render_cmd.cpp render_cmd.hpp
						render_globals_node.cpp render_globals_node.hpp)

add_library(mtoc_plugin SHARED ${sources})

target_link_libraries( mtoc_plugin cycles_bvh cycles_device cycles_render cycles_kernel cycles_subd cycles_util )
target_link_libraries( mtoc_plugin ${MAYA_Foundation_LIBRARY} ${MAYA_OpenMaya_LIBRARY} ${MAYA_OpenMayaAnim_LIBRARY} 
                      ${MAYA_OpenMayaUI_LIBRARY} ${MAYA_OpenMayaRender_LIBRARY} ${MAYA_tbb_LIBRARY} 
                      ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${OPENIMAGEIO_LIBRARY} ${Boost_LIBRARIES} )

set_target_properties( mtoc_plugin PROPERTIES OUTPUT_NAME MtoCycles CLEAN_DIRECT_OUTPUT 1 SUFFIX ".mll" )

MAYA_SET_PLUGIN_PROPERTIES( mtoc_plugin )
