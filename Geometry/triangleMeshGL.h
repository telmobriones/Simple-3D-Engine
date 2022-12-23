// -*-C++-*-

#pragma once

#include "triangleMesh.h"

class TriangleMeshGL {
public:
	static void draw(TriangleMesh * thisMesh);
private:
	static void init_opengl_vbo(TriangleMesh * thisMesh);
};
