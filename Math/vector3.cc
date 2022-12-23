#include <cstdio>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cassert>
#include "constants.h"
#include "tools.h"
#include "vector3.h"

// Constants
const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::MIN(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
const Vector3 Vector3::MAX(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

const float Vector3::epsilon = 0.0001f;

Vector3::Vector3() {
	m_v[0] = 0.0f;
	m_v[1] = 0.0f;
	m_v[2] = 1.0f;
}

Vector3::Vector3(float x, float y, float z) {
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = z;
}

Vector3::Vector3(const float *ptr) {
	m_v[0] = *ptr++;
	m_v[1] = *ptr++;
	m_v[2] = *ptr;
}

Vector3::Vector3(const Vector3 & vec) {
	m_v[0] = vec.m_v[0];
	m_v[1] = vec.m_v[1];
	m_v[2] = vec.m_v[2];
}

Vector3 & Vector3::operator=(const Vector3 & vec) {
	if (&vec != this) {
		m_v[0] = vec.m_v[0];
		m_v[1] = vec.m_v[1];
		m_v[2] = vec.m_v[2];
	}
	return *this;
}

void Vector3::swap(Vector3 & vec) {
	std::swap(m_v, vec.m_v);
}

// Coordinate access
float Vector3::operator[](int i) const { return m_v[i]; }
float & Vector3::operator[](int i) { return m_v[i]; }

// Coordinate access.
float Vector3::x () const { return m_v[0]; }
float & Vector3::x () { return m_v[0]; };
float Vector3::y () const { return m_v[1]; };
float & Vector3::y () { return m_v[1]; };
float Vector3::z () const { return m_v[2]; };
float & Vector3::z () { return m_v[2]; };

// convert to pointer to 3 flost vector
const float * Vector3::to_3fv() const { return &m_v[0]; }

// Arithmetic operations.
void Vector3::operator +=(const Vector3 & rhs) {
	m_v[0] += rhs.m_v[0];
	m_v[1] += rhs.m_v[1];
	m_v[2] += rhs.m_v[2];
}

void Vector3::operator -=(const Vector3 & rhs) {
	m_v[0] -= rhs.m_v[0];
	m_v[1] -= rhs.m_v[1];
	m_v[2] -= rhs.m_v[2];
}

void Vector3::operator /=(float scalar) {
	m_v[0] /= scalar;
	m_v[1] /= scalar;
	m_v[2] /= scalar;
}

void Vector3::operator *=(float scalar) {
	m_v[0] *= scalar;
	m_v[1] *= scalar;
	m_v[2] *= scalar;
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	return Vector3(lhs.m_v[0] + rhs.m_v[0],
				   lhs.m_v[1] + rhs.m_v[1],
				   lhs.m_v[2] + rhs.m_v[2]);
};

Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
	return Vector3(lhs.m_v[0] - rhs.m_v[0],
				   lhs.m_v[1] - rhs.m_v[1],
				   lhs.m_v[2] - rhs.m_v[2]);
};

// component-wise multiplication
Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
	return Vector3(lhs.m_v[0] * rhs.m_v[0],
				   lhs.m_v[1] * rhs.m_v[1],
				   lhs.m_v[2] * rhs.m_v[2]);
}

Vector3 operator*(const Vector3& vec, float scalar) {
	return Vector3(vec.m_v[0] *scalar,
				   vec.m_v[1] *scalar,
				   vec.m_v[2] *scalar);
}

Vector3 operator*(float scalar, const Vector3& vec) {
	return Vector3(vec.m_v[0] *scalar,
				   vec.m_v[1] *scalar,
				   vec.m_v[2] *scalar);
}

Vector3 operator/ (const Vector3& vec, float scalar) {
	assert(scalar != 0.0f);
	return Vector3(vec.m_v[0] / scalar,
				   vec.m_v[1] / scalar,
				   vec.m_v[2] / scalar);
}

float Vector3::dot(const Vector3& rhs) const {
	return m_v[0] * rhs.m_v[0] + m_v[1] * rhs.m_v[1] + m_v[2] * rhs.m_v[2];
}

// Return the projection of rhs into this
Vector3 Vector3::projection(const Vector3& rhs) const {
	float vsq = this->dot(*this);
	if (vsq < epsilon) return Vector3(); // default unit vector
	return *this * (this->dot(rhs) / vsq);
}


// cross product follows right-handed rule
Vector3 Vector3::cross(const Vector3& rhs) const {
	float x = m_v[0];
	float y = m_v[1];
	float z = m_v[2];
	return Vector3(y * rhs.m_v[2] - z * rhs.m_v[1],
				   z * rhs.m_v[0] - x * rhs.m_v[2],
				   x * rhs.m_v[1] - y * rhs.m_v[0]);
}

Vector3 crossVectors(const Vector3& lhs, const Vector3& rhs) {
	return lhs.cross(rhs);
}

float Vector3::length() const {
	return sqrtf(m_v[0] * m_v[0] + m_v[1] * m_v[1] + m_v[2] * m_v[2]);
}

float Vector3::lengthSquare() const {
	return m_v[0] * m_v[0] + m_v[1] * m_v[1] + m_v[2] * m_v[2];
}

// Set vector to unit length.
// Returns the old length
// If zero vector, set to UNIT_Z vector.
Vector3 Vector3::normalize() const{
	Vector3 res;
	float mod2 = m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2];
	if( mod2 < epsilon ) return res;
	float mod = 1.0f / sqrtf( mod2 );
	res.m_v[0] = m_v[0] * mod;
	res.m_v[1] = m_v[1] * mod;
	res.m_v[2] = m_v[2] * mod;
	return res;
}

// Normalize L1 (so that coords sum to 1)
// Returns the old length
Vector3 Vector3::normalizeL1() const{
	Vector3 res;
	float m = m_v[0] + m_v[1] + m_v[2];

	if (fabs(m) < epsilon)  return res;
	float d = (float) 1.0 / m;
	res.m_v[0] = m_v[0] * d;
	res.m_v[1] = m_v[1] * d;
	res.m_v[2] = m_v[2] * d;
	return res;
}

// Whether vector is zero
bool Vector3::isZero() const {
	return(fabs(m_v[0]) < epsilon &&
		   fabs(m_v[1]) < epsilon &&
		   fabs(m_v[2]) < epsilon);
}

// Given V = (vx, vy, vz), calculate (alpha, beta), so that:
//
// vx = |V| * cos beta * sin alfa
// vy = |V| * sin beta
// vz = |V| * cos beta * cos alfa
//
// also ,    -PI <  alpha  <  PI
//           0  <=  beta  <= PI/2
//

void Vector3::sphereCoordinates(float & alpha, float & beta) const {

	Vector3 v(*this);
	v = v.normalize();

	float modZX = Vector3(v.m_v[0], 0.0f, v.m_v[2]).length();
	if (modZX < 0.00873) {
		// If modZX is near zero beta is (+/-) PI/2 (degenerate solution) and alpha is 0.
		// 0.00873 =~ distance of ZX projection when beta is PI/2 +/- 5 degrees
		alpha = 0.0f;
		beta = Constants::pi / 2.0f;
		if( v.m_v[1] < 0.0f ) beta *= -1.0f;
	} else {
		beta = atan2f(v.m_v[1], modZX);
		alpha = atan2f(v.m_v[0], v.m_v[2]);
	}
}

void Vector3::print() const {
	printf("(%.4f, %.4f, %.4f)", m_v[0], m_v[1], m_v[2]);
};
