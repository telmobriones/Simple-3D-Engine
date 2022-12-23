// -*-C++-*-
#pragma once

#include <string>
#include "gObject.h"
#include "node.h"
#include "camera.h"
#include "shader.h"


// Create a new Skybox Node
void CreateSkybox(GObject *gobj,
				  ShaderProgram *skyShader,
				  const std::string & ctexname);
// Display Skybox Node
void DisplaySky(Camera *cam);
