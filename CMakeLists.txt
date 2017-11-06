# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Lab)

find_package(OpenGL REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall")

include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
        ${ImageMagick_INCLUDE_DIRS}
	source
        .
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
        ${ImageMagick_LIBRARIES}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(lab 
	source/main.cpp
        source/Camera.cpp
        source/LightningTechnique.cpp
        source/Object.cpp
        source/pipeline.cpp
        source/math_3d.cpp
        source/Technique.cpp
        source/Texture.cpp

        source/objloader.cpp
)
target_link_libraries(lab
	${ALL_LIBS}
)
create_target_launcher(lab WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/")
