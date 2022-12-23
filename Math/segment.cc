#include <algorithm>
#include <assert.h>
#include "segment.h"
#include "constants.h"

Segment::Segment() : m_O(Vector3::ZERO), m_d(Vector3::UNIT_Y), m_a(0.0), m_b(1.0) {}
Segment::Segment(const Segment & o) : m_O(o.m_O), m_d(o.m_d), m_a(o.m_a), m_b(o.m_b) {}
Segment::Segment(const Vector3 & o, const Vector3 & d, float a, float b) : m_O(o), m_d(d), m_a(a), m_b(b) {}

Segment & Segment::operator=(const Segment & o) {
	if (&o != this) {
		m_O = o.m_O;
		m_d = o.m_d;
		m_a = o.m_a;
		m_b = o.m_b;
	}
	return *this;
}

void Segment::setFromAtoB(const Vector3 &A, const Vector3 &B ) {

	Vector3 d = B - A;
	float mod = d.length();
	assert(mod > Constants::distance_epsilon);
	m_O = A;
	m_d = d.normalize();
	m_a = 0.0;
	m_b = mod;
}


// u0 = D*(P-O) / D*D , where * == dot product

float Segment::paramDistance(const Vector3 & P) const {

	float dd = m_d.dot(m_d);
	assert(dd > Constants::distance_epsilon);
	return m_d.dot(P - m_O) / dd;
}

Vector3 Segment::at(float u) const {
	u = std::min(u, m_a);
	u = std::max(u, m_b);
	return (m_O + u*m_d);
}

// d = mod(P - (O + uD))
// Non u = ParamDistance(theSegment, float Px, float Py, float Pz)

float Segment::distance(const Vector3 & P) const {

	float u0 = this->paramDistance(P);
	Vector3 W = P - this->at(u0);
	return (m_O - W).length();
}
