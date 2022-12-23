#include <cstdio>
#include <cmath>
#include "plane.h"
#include "constants.h"
#include "tools.h"

Plane::Plane() : m_n(Vector3::UNIT_Z), m_d(0), m_isNorm(true) {}
Plane::Plane(const Vector3 &n, float d) : m_n(n), m_d(d), m_isNorm(false) {}

void Plane::normalize() {
	float n = m_n.length();
	if (n < Constants::distance_epsilon) return;
	m_n = m_n.normalize();
	m_d  /= n;
	m_isNorm = 1;
}

// Note: first, normalize plane if necessary

void Plane::translate(const Vector3 & P) {
	if (!m_isNorm)
		normalize();
	m_d = -m_n.dot(P);
}

// Note: don't check for zero. Use DISTANCE_EPSILON instead

int Plane::whichSide(const Vector3 & P) {
	float result = m_n.dot(P) - m_d;
	if (result < -Constants::distance_epsilon ) return -1; // // onNegativeSide (inside)
	if (result > Constants::distance_epsilon ) return +1;  //onPossitiveSide (outside)
	return 0; // onPlane
}

// Note: first, normalize plane if necessary

float Plane::signedDistance(const Vector3 & P) {
	if (!m_isNorm)
		normalize();
	return m_n.dot(P) - m_d;
}

float Plane::distance(const Vector3 & P) {
	return fabs(signedDistance(P));
}
