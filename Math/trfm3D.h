// -*-C++-*-

#pragma once

#include <string>
#include "vector3.h"
#include "plane.h"

/*! \file trfm3D.h

  Geometric transformations

  Let P be a (4 x 1) column vector representing a point or vector in
  homogeneous coordinates. Then, any transformation T will be applied as:

  P' = T * P

  Therefore, the order of the transformation is from right-to-left.

  Example. Suppose we have a point P = (2,3,5) and want to a) move it by the
  translation vector (1,1,1) and b) rotate PI/4 radians about the world Y
  axis. The following code performs the desired transformations:

  <code>
  float P[3] = {2.0, 3.0, 5.0}; // Original point
  Trfm3D T;
  T.setRot_Y(PI_VALUE / 4.0); // b) rotation pi/4 about Y axis. T = Rot
  T.addTrans( Vector3(1.0, 1.0, 1.0) ); // a) translation of (1,1,1). T = Rot*Trans
  Vector3 PP =  T.transformPoint(P); // Apply the transformation PP = T * P = Rot*Trans*P
  </code>
*/


//! The matrix will be stored split into its components

/*     | c1.x*s  c2.x*s  c3.x*s tr.x | */
/* M = | c1.y*s  c2.y*s  c3.y*s tr.y | */
/*     | c1.z*s  c2.z*s  c3.z*s tr.z | */
/*     |   d.x     d.y     d.z   w   | */

class Trfm3D {

public:
	// Init with unit transformation
	Trfm3D();
	Trfm3D(const Trfm3D & T);
	void swap(Trfm3D & T);

	/*! Transformation composition : add one trfm onto another one
	  this = this * add */

	void add(const Trfm3D & T);
	void add(const Trfm3D * T);

	// More transformations

	friend Trfm3D operator *(const Trfm3D &lhs, const Trfm3D &rhs);
	void operator *=(const Trfm3D *rhs); // same as add
	void operator *=(const Trfm3D &rhs); // same as add

	// clone transformation from T
	void clone(const Trfm3D &T );
	void clone(const Trfm3D *T );

	//! Transform a point P by transformation T
	/*! Return the point P transformed by trfm T, by post-multiplying the matrix T to the column-point P
	*/
	Vector3 transformPoint(const Vector3 & P) const;

	//! Transform a vector V by transformation T
	/*! Return the vector V transformed by trfm T, by post-multiplying the matrix T to the column-vector V.

	  Note: vectors do not translate
	*/
	Vector3 transformVector(const Vector3 & V) const;

	/**
	 * Transform a normal
	 */
	Vector3 transformNormal(const Vector3 & N) const;

	/**
	 * Transform a (normalized) plane given by N*X = d
	  Leave the result in pl
	 */
	void transformPlane(Plane * pl) const;

	/**
	 * Project a point P by proyection transformation
	  Leave the result in P
	 */

	Vector3 projectPoint(const Vector3 & P) const;

	/*! Gets an OpenGl compatible matrix with a Trfm3D.

	  glmatrix must have enough space to store the values of the matrix (16)
	*/
	void getGLMatrix(float *glmatrix ) const;

	// Set functions
	void setUnit();  // Set the trfm to the unit transformation
	void setRotX(float angle ); // Sets the trfm to be a rotation of angle radians in the X axis
	void setRotY(float angle ); // Sets the trfm to be a rotation of angle radians in the Y axis
	void setRotZ(float angle ); // Sets the trfm to be a rotation of angle radians in the Z axis
	void setTrans(const Vector3 & tr); // Sets the trfm to be a translation T
	void setScale(float scale ); // Sets the trfm to be a uniform scale
	void setRotVec(const Vector3 & v, float theta ); //!< Sets the trfm to be a rotation of theta radians on arbitrary axis v
	//! Set the trfm to rotate angle radians about an axis defined by vector V and
	//! located at point P
	/*!
	  \note V must be an unit vector
	*/
	void setRotAxis(const Vector3 & V, const Vector3 & P, float angle );

	/**
	 * Set the trfm so that it performs a parallel projection given by the
	 * cube parameters
	 */
	void setOrtho(float left, float right,
				  float bottom, float top,
				  float near, float far);

	/**
	 * Set the trfm so that it performs a perspective projection given by the
	 * frustum parameters
	 */
	void setFrustum(float left, float right,
					float bottom, float top,
					float near, float far);

	/**
	 * Given a point P, a right vector R, an up vector U, and a vector D looking
	 * backwards, all in world coordinates, create a Local-to-World
	 * transformation.
	 *
	 \note: R,U and D have to be unit lenght and orthogonal
	*/

	void setLocal2World(const Vector3 & P, // origin
						const Vector3 & R, // right
						const Vector3 & U, // up
						const Vector3 & D);// backwards

	/**
	 * Given a point (Px,Py,Pz), a right vector (Rx, Ry, Rz), an up vector (Ux,
	 * Uy, Uz) and a vector looking backwards (Dx, Dy, Dz), all in world
	 * coordinates, create a World-to-Local transformation.
	*/

	void setWorld2Local(const Vector3 & P, // origin
						const Vector3 & R, // right
						const Vector3 & U, // up
						const Vector3 & D);// backwards

	/**
	 * Given a trfm that represents a local frame, create a World-to-Local
	 * transformation
	 *
	 * @param thisTrfm3D pointer to a transformation (can't be 0)
	 * @param frameTrfm  a Trfm representing a frame
	 */

	void setWorld2LocalFrame(const Trfm3D &frameTrfm);
	void setWorld2LocalFrame(const Trfm3D *frameTrfm);

	/**
	 * Normalize a trfm so that the three rotation vectors are unit lenght and
	 * mutually orthogonal
	 */

	void normalize();
	void setInverse();	//!< Sets the matrix to its inverse

	// Add functions

	void addRotX(float angle ); //!< Add the specified trfm a rotation of angle radians in the X axis
	void addRotY(float angle ); //!< Add the specified trfm a rotation of angle radians in the Y axis
	void addRotZ(float angle ); //!< Add the specified trfm a rotation of angle radians in the Z axis
	void addRotVec(const Vector3 & V, float theta ); //!< add to the trfm a rotation of theta radians on arbitrary axis V
	void addRotAxis(const Vector3 & V, const Vector3 & P, float theta); //!< add to the trfm a rotation of theta radians on arbitrary axis R located at point P

	void addTrans(const Vector3 & P); //!< Add the specified trfm a translation T
	void addScale(float scale ); //!< Add the specified trfm a uniform scale

	void addFrustum(float left, float right,
					float top, float bottom,
					float near, float far);

	void addWorld2Local(const Vector3 & P,
						const Vector3 & R,
						const Vector3 & U,
						const Vector3 & D);
	void addLocal2World(const Vector3 & P,
						const Vector3 & R,
						const Vector3 & U,
						const Vector3 & D);

	//! Get translation component of Trfm3D

	Vector3 getTranslation() const;

	/*!
	  This function transforms all the negative components to positive ones
	*/

	int cmp(const Trfm3D & T) const;
	int cmp(const Trfm3D * T) const;
	void abs();
	void print(const std::string & del = std::string()) const;

private:
	Vector3 m_c1, m_c2, m_c3;
	Vector3 m_tr;
	Vector3 m_d;
	float m_scl;
	float  m_w;
};
