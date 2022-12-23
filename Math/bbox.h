// -*-C++-*-

#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "vector3.h"
#include "trfm3D.h"

class BBox {

public:

	BBox();
	BBox(const Vector3 & min, const Vector3 & max);
	~BBox();
	/**
	 * Set void BBox
	 */
	void init();

	// swap two BBoxes
	void swap(BBox & rhs);

	/**
	 * Copy from source bounding box
	 */
	void clone(const BBox * source);

	/**
	 * add a point to the  BBox
	 *
	 *
	 */
	void add(const Vector3 & P );
	/**
	 * include: modify BBox to include other
	 *
	 *
	 */
	void include(const BBox *other );

	/**
	 * transform by a trfm
	 */
	void transform(const Trfm3D * T);


	void print() const;

	Vector3  m_min;
	Vector3  m_max;

	// VBO
	GLuint  m_vbo_id;    // Vertex Buffer Object id
	GLuint  m_idxvbo_id; // Index Vertex Buffer Object id
	int     m_vbo_uptodate;
};
