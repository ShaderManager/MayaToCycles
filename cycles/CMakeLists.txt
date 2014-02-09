if(WITH_CYCLES_NETWORK)
  add_definitions(-DWITH_NETWORK)
endif()

if(WITH_CYCLES_OSL)
  add_definitions(-DWITH_OSL)
endif()

if(WITH_CYCLES_PARTIO)
  add_definitions(-DWITH_PARTIO)
endif()

add_definitions(-DWITH_OPENCL)
add_definitions(-DWITH_CUDA)
add_definitions(-DWITH_MULTI)

add_subdirectory(bvh)
add_subdirectory(device)
add_subdirectory(kernel)
add_subdirectory(render)
add_subdirectory(subd)
add_subdirectory(util)
