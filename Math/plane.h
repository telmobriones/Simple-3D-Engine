// -*-C++-*-

#pragma once

#include "vector3.h"

/*!
  \file plane.h

  Planes

  The plane internally using implicit formulation, the eq. of the plane is:

  N*X = d

*/

class Plane {

public:

	Plane(); // Plane aligned with the XY plane: n=(0,0,1) d=0

	/*!
	  Given 4 algebraic coefficients (a,b,c,l) , create a plane with eq.

	  N*X = d

	  where N is a unit length normal N and d a distance

	  \return the new normalized plane
	*/
	Plane(const Vector3 & n, float d);

	/**
	 * Translates a plane so that it passes through P
	 *
	 * \note the plane is normalized
	 */

	void translate(const Vector3 & P);

	/**
	 * Normalize the plane (normalize plane normal and set distance accordingly)
	 */
	void normalize();

	//! Decides in which side a point lies respect to a plane
	/*!
	  \return
	  - 0  point lies on the plane
	  - +1 point lies outside the plane (possitive side)
	  - -1 point lies inside the plane (negative side)

	  \note the plane must not be normalized
	*/
	int whichSide(const Vector3 & P);

	/**
	 * Calculates the distance of a Point w.r.t a plane.
	 *
	 *
	 * @return the distance to the plane. Always positive.
	 *
	 * \note the plane is normalized
	 */
	float distance(const Vector3 & P);

	/**
	 * Calculates the signed distance of a Point w.r.t a plane. If the return value
	 * is negative, it tells that the point is on the back-side of the plane.
	 *
	 * @return the signed distance to the plane.
	 *
	 * \note the plane is normalized
	 */
	float signedDistance(const Vector3 & P);

	//! Plane normal
	Vector3 m_n;
	//! Distance
	float m_d;
	//! whether the plane is normalized
	bool  m_isNorm;
};
