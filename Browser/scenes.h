// -*-C++-*-
#pragma once

#include "node.h"

Node *create_procedural_scene(const std::string & name,
							  int Width, int Height);
Node *create_scene();
Node *create_scene_city();
Node *create_scene_city_bmap();
Node *create_scene_noT();
Node *create_scene_triang();
Node *create_scene_bmap();
