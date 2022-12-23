cmake_minimum_required (VERSION 3.10)
project (mgMathTest)

# opengl stuff
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})

set(SOURCES 
  vector3.cc trfm3D.cc plane.cc line.cc bbox.cc intersect.cc bsphere.cc ../Misc/tools.cc ../Misc/constants.cc)

include_directories (. ../Math ../Misc)
add_executable(test test.cc ${SOURCES})
target_link_libraries(test OpenGL::OpenGL ${GLUT_LIBRARY} GLU GLEW::GLEW)
