// -*-C++-*-

#pragma once

#include "vector3.h"

class BSphere {

public:

	BSphere();
	// Create a new BSphere located at (x,y,z) and radius 'radius'
	BSphere(const Vector3 & centre, float radius );
	BSphere(const BSphere & sphere);
	BSphere & operator=(const BSphere & sphere);

	const Vector3 & getPosition() const;
	float getRadius() const;

	void setPosition(const Vector3 & newpos); // Place an existing BSphere at newpos
	void setRadius(float r);

	Vector3  m_centre;
	float    m_radius;
};
