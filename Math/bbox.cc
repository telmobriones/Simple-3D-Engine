#include <cstdio>
#include <limits>
#include <algorithm>
#include <cmath>
#include "tools.h"
#include "bbox.h"

BBox::~BBox() {
	// reclaim openGL buffers
	if (m_vbo_id)
		glDeleteBuffers(1, &m_vbo_id);
	if (m_idxvbo_id)
		glDeleteBuffers(1, &m_idxvbo_id);
}

BBox::BBox() : m_min(Vector3::MAX), m_max(-1*Vector3::MAX), m_vbo_id(0), m_idxvbo_id(0), m_vbo_uptodate (0) {}

BBox::BBox(const Vector3 & min, const Vector3 & max) : m_min(min), m_max(max) {
	m_vbo_id = 0;
	m_idxvbo_id = 0;
	m_vbo_uptodate = 0;
}

void BBox::clone(const BBox * source) {
	m_min = source->m_min;
	m_max = source->m_max;
	m_vbo_uptodate = 0;
}

void BBox::init() {
	BBox().swap(*this);
}

void BBox::swap(BBox & rhs) {
	m_min.swap(rhs.m_min);
	m_max.swap(rhs.m_max);
	std::swap(m_vbo_id, rhs.m_vbo_id);
	std::swap(m_idxvbo_id, rhs.m_idxvbo_id);
	std::swap(m_vbo_uptodate, rhs.m_vbo_uptodate);
}

void BBox::add(const Vector3 & P) {
	m_min[0] = fmin(m_min[0], P[0]);
	m_min[1] = fmin(m_min[1], P[1]);
	m_min[2] = fmin(m_min[2], P[2]);
	m_max[0] = fmax(m_max[0], P[0]);
	m_max[1] = fmax(m_max[1], P[1]);
	m_max[2] = fmax(m_max[2], P[2]);
	m_vbo_uptodate = 0;
}


void BBox::include(const BBox *bbb ) {
	add(bbb->m_min);
	add(bbb->m_max);
	m_vbo_uptodate = 0;
}

/* Given a trfm3D transformation, calculate the new axis aligned BBox

   Algorithm based on:
   Transforming Axis-Aligned Bounding Boxes
   by Jim Arvo
   from "Graphics Gems", Academic Press, 1990
*/

void BBox::transform(const Trfm3D * T) {

	float  a, b;
	float  Amin[3], Amax[3];
	float  Bmin[3], Bmax[3];
	int    i, j;

	static float M[16]; // OpenGL matrix. Column-major mode !

	T->getGLMatrix(&M[0]); // Copy transf. to array

	/*Copy box geomBox into a min array and a max array for easy reference.*/

	Amin[0] = m_min[0];  Amax[0] = m_max[0];
	Amin[1] = m_min[1];  Amax[1] = m_max[1];
	Amin[2] = m_min[2];  Amax[2] = m_max[2];

	/* Take care of translation by beginning at T. */

	Bmin[0] = Bmax[0] = M[12]; // T->trX
	Bmin[1] = Bmax[1] = M[13]; // T->trY
	Bmin[2] = Bmax[2] = M[14]; // T->trZ

	/* Now find the extreme points by considering the product of the */
	/* min and max with each component of M.  */

	for( i = 0; i < 3; i++ )
		for( j = 0; j < 3; j++ ) {
			a = M[i + 4*j] * Amin[j]; // M.element[i][j] * Amin[j];
			b = M[i + 4*j] * Amax[j]; // M.element[i][j] * Amax[j];
			if( a < b )  {
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else {
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}

	/* Copy the result into the new box. */

	m_min[0] = Bmin[0];
	m_min[1] = Bmin[1];
	m_min[2] = Bmin[2];

	m_max[0] = Bmax[0];
	m_max[1] = Bmax[1];
	m_max[2] = Bmax[2];

	m_vbo_uptodate = 0;
}


void BBox::print() const {
	printf("(%.2f %.2f %.2f), (%.2f %.2f %.2f)", m_min[0], m_min[1], m_min[2], m_max[0], m_max[1], m_max[2]);
}
