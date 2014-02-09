cmake_minimum_required(VERSION 2.8)

project(MtoCycles)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin )

# External Libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS thread filesystem date_time system chrono REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenImageIO REQUIRED)
find_package(Maya REQUIRED)

include(cmake/external_libs.cmake)

# pair of macros to allow libraries to be specify files to install, but to
# only install them at the end so the directories don't get cleared with
# the files in them. used by cycles to install addon.
macro(delayed_install
	base
	files
	destination)

	foreach(f ${files})
		set_property(GLOBAL APPEND PROPERTY DELAYED_INSTALL_FILES ${base}/${f})
		set_property(GLOBAL APPEND PROPERTY DELAYED_INSTALL_DESTINATIONS ${destination})
	endforeach()
endmacro()

# Build Flags

set(GCC_WARNING_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-long-long")
set(GCC_OPTIM_FLAGS "-ffast-math -msse -msse2 -msse3 -mtune=native")

if(APPLE)
	set(CMAKE_CXX_FLAGS "${GCC_WARNING_FLAGS} ${GCC_OPTIM_FLAGS}")
	set(RTTI_DISABLE_FLAGS "-fno-rtti -DBOOST_NO_RTTI -DBOOST_NO_TYPEID")
endif(APPLE)

if(WIN32)
	if(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /Ot /arch:SSE2 -D_CRT_SECURE_NO_WARNINGS /EHsc /fp:fast")
		set(RTTI_DISABLE_FLAGS "/GR- -DBOOST_NO_RTTI -DBOOST_NO_TYPEID")
	elseif(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_CXX_FLAGS "${GCC_WARNING_FLAGS} ${GCC_OPTIM_FLAGS}")
		set(RTTI_DISABLE_FLAGS "-fno-rtti -DBOOST_NO_RTTI -DBOOST_NO_TYPEID")
	endif()
endif(WIN32)

if(UNIX AND NOT APPLE)
	set(CMAKE_CXX_FLAGS "${GCC_WARNING_FLAGS} ${GCC_OPTIM_FLAGS}")
	set(RTTI_DISABLE_FLAGS "-fno-rtti -DBOOST_NO_RTTI -DBOOST_NO_TYPEID")
endif(UNIX AND NOT APPLE)

# Definitions and Includes

add_definitions(${BOOST_DEFINITIONS} ${OPENIMAGEIO_DEFINITIONS})

add_definitions(-DCCL_NAMESPACE_BEGIN=namespace\ ccl\ {)
add_definitions(-DCCL_NAMESPACE_END=})

include_directories(
	${Boost_INCLUDE_DIR}
	${OPENIMAGEIO_INCLUDES}
	${OPENIMAGEIO_INCLUDES}/OpenImageIO)

# Subdirectories
add_subdirectory(cycles)
add_subdirectory(mtoc)

