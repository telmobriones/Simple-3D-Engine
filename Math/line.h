// -*-C++-*-

#pragma once

#include "vector3.h"

/*!
  \file line.h

  Lines

  The line internally is stored in parametric form, by means of a point O =
  (ox,oy,oz) and a direction vector D = (dx,dy,dz)

  The eq. of the line is:

  p(u) = O + uD

*/

class Line {

 public:

	Line();
	//! Create a line given a point and a direction vector
	//   p(u) = O + uD
	Line(const Vector3 & o, const Vector3 & d);
	Line(const Line & line);
	Line & operator=(const Line & line);

	// Set line to pass through two points A and B
	void setFromAtoB(const Vector3 & A, const Vector3 & B);

	//! Give the point corresponding to parameter u

	Vector3 at(float u) const;

	//! Minimum distance between line and a point
	float distance(const Vector3 & P) const;

	//! Given a point P, return the value of parameter u nearest the point
	float paramDistance(const Vector3 & P) const;

	//! Print line

	void print() const;

	//! A point o
	Vector3 m_O;
	//! A direction vector d
	Vector3 m_d;
};
