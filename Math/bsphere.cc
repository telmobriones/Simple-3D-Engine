#include "bsphere.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif

BSphere::BSphere() : m_centre(Vector3::ZERO), m_radius(0.0f) {}
BSphere::BSphere(const Vector3 & centre, float radius ) : m_centre(centre), m_radius(radius) {}
BSphere::BSphere(const BSphere & sphere) : m_centre(sphere.m_centre), m_radius(sphere.m_radius) {}

BSphere & BSphere::operator=(const BSphere & sphere) {
	if (&sphere != this) {
		m_centre = sphere.m_centre;
		m_radius = sphere.m_radius;
	}
	return *this;
}

const Vector3 & BSphere::getPosition() const { return m_centre; }
float BSphere::getRadius() const { return m_radius; }

void BSphere::setPosition(const Vector3 & newpos) {
	m_centre = newpos;
}
void BSphere::setRadius(float r) { m_radius = r; }
