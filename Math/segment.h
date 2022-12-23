// -*-C++-*-

#pragma once

#include "vector3.h"

/*!
  \file segment.h

  Segments

  The segment internally is stored by means of a point O = (ox,oy,oz) and a
  direction vector D = (dx,dy,dz)

  The eq. of the segment is:

  p(u) = O + uD

*/

class Segment {

public:
	//! Create empty segment
	Segment();

	//! Create a segment given a point and a direction vector
	/*!
	  Given a direction vector D = (dx,dy,dz) and a point O = (ox,oy,oz) ,
	  create a segment with eq.

	  p(u) = O + uD , a <= u <= b

	*/
	Segment(const Vector3 & o, const Vector3 & d, float a, float b);

	Segment(const Segment &);
	Segment & operator=(const Segment & o);

	//! Return point at p
	Vector3 at(float p) const;

	//! Given two points, A and B, set segment to go from A to B
	void setFromAtoB(const Vector3 & A, const Vector3 & B);

	//! Distance between the segment and a point
	float distance(const Vector3 & P) const;

	//! Given a point P, return the value of parameter u in [a,b] nearest the point
	float paramDistance(const Vector3 & P) const;

private:
	//! A point o
	Vector3 m_O;
	//! A direction vector d
	Vector3 m_d;
	float m_a,m_b; // limits
};
