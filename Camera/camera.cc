#include <cstdio>
#include <cmath>
#include "camera.h"
#include "constants.h"
#include "tools.h"
#include "intersect.h"

static void do_tr(const Trfm3D &T, Vector3 &V, bool isP = false);

Camera::Camera() : m_viewTrfm(new Trfm3D),
				   m_projTrfm(new Trfm3D),
				   m_lastE(Vector3::ZERO),
				   m_lastAt(0.0f, 0.0f, -1.0f),
				   m_lastUp(0.0f, 0.0f, -1.0f),
				   m_E(Vector3::ZERO),
				   m_R(Vector3::UNIT_X),
				   m_U(Vector3::UNIT_Y),
				   m_D(Vector3::UNIT_Z)
{
	savePosition();
	for (int i = 0; i < MAX_CLIP_PLANES; ++i)
		m_fPlanes[i] = new Plane();
}

Camera::~Camera()
{
	delete m_viewTrfm;
	delete m_projTrfm;
	for (int i = 0; i < MAX_CLIP_PLANES; ++i)
		delete m_fPlanes[i];
}

void Camera::restorePosition()
{
	m_E = saved_pos.m_E;
	m_R = saved_pos.m_R;
	m_U = saved_pos.m_U;
	m_D = saved_pos.m_D;
}

void Camera::savePosition()
{
	saved_pos.m_E = m_E;
	saved_pos.m_R = m_R;
	saved_pos.m_U = m_U;
	saved_pos.m_D = m_D;
}

PerspectiveCamera::PerspectiveCamera() : m_fovy(60.0f * Constants::degree_to_rad)
{
	m_type = perspective;
	m_near = 0.1f;
	m_far = 1500.0f;
	m_aspectRatio = 1.0f;
	m_left = -0.034451;
	m_bottom = -0.026795;
	m_right = 0.034451;
	m_top = 0.026795;
	m_projTrfm->setFrustum(m_left, m_right,
						   m_bottom, m_top,
						   m_near, m_far);
}

PerspectiveCamera::~PerspectiveCamera() {}

void PerspectiveCamera::init(float fovy, float aspect,
							 float near, float far)
{
	m_fovy = fovy;
	m_aspectRatio = aspect;
	m_near = near;
	m_far = far;
	updateProjection();
}

OrthographicCamera::OrthographicCamera()
{
	m_type = orthographic;
	m_near = 0.1f;
	m_far = 1500.0f;
	m_left = -0.034451;
	m_bottom = -0.026795;
	m_right = 0.034451;
	m_top = 0.026795;
	m_projTrfm->setOrtho(m_left, m_right,
						 m_bottom, m_top,
						 m_near, m_far);
}

OrthographicCamera::~OrthographicCamera() {}

void OrthographicCamera::init(float left, float right,
							  float bottom, float top,
							  float near, float far)
{
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
	m_near = near;
	m_far = far;
	updateProjection();
}

////////////////////////////////////////////////////////////7
// Projection stuff

void OrthographicCamera::onResize(int w, int h) {}
void PerspectiveCamera::onResize(int w, int h)
{
	if (h > 0)
		m_aspectRatio = (float)w / (float)h; // width/height
	else
		m_aspectRatio = 1.0f;
	updateProjection();
}

// zoom camera

void Camera::zoom(float angle) {}
void PerspectiveCamera::zoom(float angle)
{
	m_fovy = angle;
	updateProjection();
}
void OrthographicCamera::zoom(float factor)
{
	m_left *= factor;
	m_right *= factor;
	m_bottom *= factor;
	m_top *= factor;
	updateProjection();
}

float Camera::getZoom() const { return 0.0f; }
float PerspectiveCamera::getZoom() const { return m_fovy; }

void OrthographicCamera::updateProjection()
{
	m_projTrfm->setOrtho(m_left, m_right,
						 m_bottom, m_top,
						 m_near, m_far);
	updateFrustumPlanes();
}

// @@ TODO:
//
// * Given (m_near, m_far, m_aspectRatio, m_fovy):
//
//   - calculate the values of frustum (m_top, m_bottom, m_right, m_left)
//   - update projection matrix (m_projTrfm)

void PerspectiveCamera::updateProjection()
{
	/* =================== PUT YOUR CODE HERE ====================== */
	m_top = m_near * tan(m_fovy / 2);
	m_bottom = -m_top;
	m_right = m_aspectRatio * m_top;
	m_left = -m_right;
	m_projTrfm->setFrustum(m_left, m_right,
						   m_bottom, m_top,
						   m_near, m_far);
	/* =================== END YOUR CODE HERE ====================== */
	updateFrustumPlanes();
}

////////////////////////////////////////////////////////////7
// View transformation stuff

void Camera::setViewTrfm()
{
	m_viewTrfm->setWorld2Local(m_E, m_R, m_U, m_D);
	updateFrustumPlanes();
}

void Camera::updateFrame()
{
	setViewTrfm();
	m_viewTrfm->normalize();
}

// @@ TODO:
/**
 * Calculate (m_E, m_R, m_U, m_D) vectors of the camera frame given (E, at, up)
 *
 */

void Camera::lookAt(const Vector3 &E,
					const Vector3 &at,
					const Vector3 &up)
{
	/* =================== PUT YOUR CODE HERE ====================== */

	m_E = E;
	m_D = (E - at) / (E - at).length();
	m_R = (up / up.length()).cross(m_D);
	m_U = m_D.cross(m_R);

	/* =================== END YOUR CODE HERE ====================== */
	setViewTrfm();
}

////////////////////////////////////////////////////////////7
// get/set

const char *Camera::getName() const { return m_name.c_str(); };
const std::string &Camera::getNameString() const { return m_name; };
void Camera::setName(const std::string &n) { m_name = n; };

void Camera::setFar(float far)
{
	m_far = far;
	updateProjection();
}
float Camera::getFar() const { return m_far; }
void Camera::setNear(float near)
{
	m_near = near;
	updateProjection();
}
float Camera::getNear() const { return m_near; }
Vector3 Camera::getPosition() const { return m_E; }
Vector3 Camera::getDirection() const { return -1.0f * m_D; }
Vector3 Camera::getUp() const { return m_U; }

////////////////////////////////////////////////
// trfm transformations

const Trfm3D *Camera::viewTrfm() const
{
	return m_viewTrfm;
	;
}
const Trfm3D *Camera::projectionTrfm() const { return m_projTrfm; }
void Camera::viewTrfmGL(float *gmatrix) const { m_viewTrfm->getGLMatrix(gmatrix); }
void Camera::projectionTrfmGL(float *gmatrix) const { m_projTrfm->getGLMatrix(gmatrix); }

////////////////////////////////////////////////
// Movement

// Move the camera "step" units ahead. Fly mode.
//
// thisCamera         -> the camera
// step               -> number of units to fly (can be negative)

void Camera::fly(float step)
{

	m_E -= step * m_D;
	setViewTrfm();
}

// Move the camera "step" units ahead. Walk mode.
//
// thisCamera         -> the camera
// step               -> number of units to walk (can be negative)

void Camera::walk(float step)
{

	m_E[0] -= step * m_D[0];
	m_E[2] -= step * m_D[2];
	setViewTrfm();
}

void Camera::panX(float step)
{
	m_E += step * m_R;
	setViewTrfm();
}

void Camera::panY(float step)
{
	m_E += step * m_U;
	setViewTrfm();
}

void Camera::pitch(float angle)
{
	Trfm3D T;
	T.setRotVec(m_R, angle);
	do_tr(T, m_D);
	do_tr(T, m_U);
	updateFrame();
}

void Camera::yaw(float angle)
{
	Trfm3D T;
	T.setRotVec(m_U, angle);
	do_tr(T, m_R);
	do_tr(T, m_D);
	updateFrame();
}

void Camera::roll(float angle)
{
	Trfm3D T;
	T.setRotVec(m_D, -angle);
	do_tr(T, m_R);
	do_tr(T, m_U);
	updateFrame();
}

// Rotate camera around world's Y axis

void Camera::viewYWorld(float angle)
{
	Trfm3D T;
	T.setRotVec(Vector3::UNIT_Y, angle);
	do_tr(T, m_R);
	do_tr(T, m_U);
	do_tr(T, m_D);
	updateFrame();
}

void Camera::arcOverUnder(float angle)
{
	Trfm3D T;
	Vector3 at(m_E - 100.0 * m_D);
	T.setRotAxis(m_R, at, angle);
	do_tr(T, m_U);
	do_tr(T, m_D);
	do_tr(T, m_E, true);
	updateFrame();
}

void Camera::arcLeftRight(float angle)
{
	Trfm3D T;
	Vector3 at(m_E - 100.0 * m_D);
	T.setRotAxis(Vector3::UNIT_Y, at, angle);
	do_tr(T, m_U);
	do_tr(T, m_R);
	do_tr(T, m_D);
	do_tr(T, m_E, true);
	updateFrame();
}

// @@ TODO: Check frustum (look at camera.h for parameter descriptions and return
//          values)

int Camera::checkFrustum(const BBox *theBBox)
{
	//,unsigned int *planesBitM) {
	/* =================== PUT YOUR CODE HERE ====================== */
	for (int i = 0; i < 6; ++i)
	{
		if (BBoxPlaneIntersect(theBBox, m_fPlanes[i]) == +1)
			return +1; // BBox is fully outside the frustum
		else if (BBoxPlaneIntersect(theBBox, m_fPlanes[i]) == 0)
			return 0; // BBox is intersecting the frustum
	}
	return -1; // BBox is fully inside the frustum
			   /* =================== END YOUR CODE HERE ====================== */
}

/////////////////////////////////////////////////////////////////////////////////////
// Hemendik aurrera ez

void Camera::updateFrustumPlanes()
{

	static GLfloat M[16];
	Plane *p;

	// V: view transformation
	// P: perspective transformation
	// T = P*V

	Trfm3D T(*m_projTrfm); // T = P;
	T.add(m_viewTrfm);	   // T = P*V
	T.getGLMatrix(M);

	// Extract the planes parameters

	// left plane
	p = m_fPlanes[0];

	p->m_n[0] = -M[3] - M[0];  // -(m_41 + m_11)
	p->m_n[1] = -M[7] - M[4];  // -(m_42 + m_12)
	p->m_n[2] = -M[11] - M[8]; // -(m_43 + m_13)
	p->m_d = M[15] + M[12];	   //  (m_44 + m_14), because d in plane is really (-d)
	p->m_isNorm = false;

	// right plane
	p = m_fPlanes[1];
	p->m_n[0] = -M[3] + M[0];  // -(m_41 - m_11)
	p->m_n[1] = -M[7] + M[4];  // -(m_42 - m_12)
	p->m_n[2] = -M[11] + M[8]; // -(m_43 - m_13)
	p->m_d = M[15] - M[12];	   //  (m_44 - m_14) because d in plane is really (-d)
	p->m_isNorm = false;

	// bottom plane
	p = m_fPlanes[2];
	p->m_n[0] = -M[3] - M[1];  // -(m_41 + m_21)
	p->m_n[1] = -M[7] - M[5];  // -(m_42 + m_22)
	p->m_n[2] = -M[11] - M[9]; // -(m_43 + m_23)
	p->m_d = M[15] + M[13];	   //  (m_44 + m_24) because d in plane is really (-d)
	p->m_isNorm = false;

	// top plane
	p = m_fPlanes[3];
	p->m_n[0] = -M[3] + M[1];  // -(m_41 - m_21)
	p->m_n[1] = -M[7] + M[5];  // -(m_42 - m_22)
	p->m_n[2] = -M[11] + M[9]; // -(m_43 - m_23)
	p->m_d = M[15] - M[13];	   //  (m_44 - m_24) because d in plane is really (-d)
	p->m_isNorm = false;

	// near plane
	p = m_fPlanes[4];
	p->m_n[0] = -M[3] - M[2];	// -(m_41 + m_31)
	p->m_n[1] = -M[7] - M[6];	// -(m_42 + m_32)
	p->m_n[2] = -M[11] - M[10]; // -(m_43 + m_33)
	p->m_d = M[15] + M[14];		//  (m_44 + m_34) because d in plane is really (-d)
	p->m_isNorm = false;

	// far plane
	p = m_fPlanes[5];
	p->m_n[0] = -M[3] + M[2];	// -(m_41 - m_31)
	p->m_n[1] = -M[7] + M[6];	// -(m_42 - m_32)
	p->m_n[2] = -M[11] + M[10]; // -(m_43 - m_33)
	p->m_d = M[15] - M[14];		//  (m_44 - m_34) because d in plane is really (-d)
	p->m_isNorm = false;
	// It is not neccesary to normailze the planes for frustum calculation
}

void Camera::print()
{

	printf("View Point    : %.4f %.4f %.4f\n", m_E[0], m_E[1], m_E[2]);
	printf("Frame\n");
	printf("R : %.4f %.4f %.4f\n", m_R[0], m_R[1], m_R[2]);
	printf("U : %.4f %.4f %.4f\n", m_U[0], m_U[1], m_U[2]);
	printf("D : %.4f %.4f %.4f\n", m_D[0], m_D[1], m_D[2]);
	printf("View trfm:\n");
	m_viewTrfm->print();
}

void PerspectiveCamera::print()
{
	printf("*** Perspective camera: %s\n", m_name.size() ? m_name.c_str() : "(noname)");
	Camera::print();
	printf("Fovy %.4f Near %.4f Far %.4f\n", m_fovy * Constants::rad_to_degree,
		   m_near, m_far);
	printf("Ortho l,b,n min  %.4f %.4f %.4f\n", m_left, m_bottom, m_near);
	printf("      r,t,f max  %.4f %.4f %.4f\n", m_right, m_top, m_far);
	printf("Projection trfm:\n");
	m_projTrfm->print();
}

void OrthographicCamera::print()
{
	printf("*** Orthographic camera: %s\n", m_name.size() ? m_name.c_str() : "(noname)");
	Camera::print();
	printf("Ortho l,b,n min  %.4f %.4f %.4f\n", m_left, m_bottom, m_near);
	printf("      r,t,f max  %.4f %.4f %.4f\n", m_right, m_top, m_far);
	printf("Projection trfm:\n");
	m_projTrfm->print();
}

void do_tr(const Trfm3D &T, Vector3 &V, bool isP)
{
	float m[16];
	T.getGLMatrix(&m[0]);
	Vector3 res(V[0] * m[0] + V[1] * m[4] + V[2] * m[8],
				V[0] * m[1] + V[1] * m[5] + V[2] * m[9],
				V[0] * m[2] + V[1] * m[6] + V[2] * m[10]);
	if (isP)
	{
		res += Vector3(m[12], m[13], m[14]);
	}
	V.swap(res);
}
