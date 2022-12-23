cmake_minimum_required (VERSION 3.10)
project (mg)

if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

if (WIN32)
  message(ERROR " Windows not supported")
  return()
endif (WIN32)

set(CMAKE_BUILD_TYPE Debug)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# opengl stuff
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})

set(SOURCES 
  Math/vector3.cc Math/trfm3D.cc Math/plane.cc Math/line.cc Math/segment.cc
  Math/bbox.cc Math/bsphere.cc Math/intersect.cc
  Math/bboxGL.cc Math/trfmStack.cc
  Geometry/triangleMesh.cc Geometry/gObject.cc Geometry/gObjectManager.cc
  Geometry/triangleMeshGL.cc
  Shading/light.cc Shading/material.cc Shading/texture.cc Shading/texturert.cc
  Shading/image.cc Shading/textureManager.cc Shading/materialManager.cc
  Shading/lightManager.cc Shading/imageManager.cc
  Shaders/shaderUtils.cc Shaders/shaderManager.cc Shaders/shader.cc Shaders/uniform.cc
  Camera/camera.cc Camera/avatar.cc Camera/cameraManager.cc Camera/avatarManager.cc
  Scene/node.cc Scene/nodeManager.cc Scene/renderState.cc Scene/scene.cc Scene/skybox.cc
  Misc/constants.cc Misc/tools.cc Misc/jsoncpp.cpp Misc/parse_scene.cc
  Misc/glm.c
  Browser/scenes.cc)
include_directories (. Math Geometry Shading Shaders Camera Scene Misc Browser)

add_executable(browser Browser/browser.cc ${SOURCES})
add_executable(browser_gobj Browser/browser_gobj.cc ${SOURCES})
target_link_libraries(browser OpenGL::OpenGL ${GLUT_LIBRARY} GLU GLEW::GLEW)
target_link_libraries(browser_gobj OpenGL::OpenGL ${GLUT_LIBRARY} GLU GLEW::GLEW)

