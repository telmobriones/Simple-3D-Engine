#include <cmath>
#include "intersect.h"
#include "constants.h"
#include "tools.h"

/* | algo           | difficulty | */
/* |----------------+------------| */
/* | BSPherePlane   |          1 | */
/* | BBoxBBox       |          2 | */
/* | BBoxPlane      |          4 | */

// @@ TODO: test if a BSpheres intersects a plane.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int BSpherePlaneIntersect(const BSphere *bs, Plane *pl)
{
	/* =================== PUT YOUR CODE HERE ====================== */
	float d = pl->distance(bs->m_centre);
	if (d <= bs->m_radius)
	{
		return IINTERSECT;
	}
	else if (pl->whichSide(bs->m_centre) == +1)
	{
		return +IREJECT; // Outside
	}
	else
	{
		return -IREJECT; // Inside
	}
	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO: test if two BBoxes intersect.
//! Returns :
//    IINTERSECT intersect
//    IREJECT don't intersect

int BBoxBBoxIntersect(const BBox *bba, const BBox *bbb)
{
	/* =================== PUT YOUR CODE HERE ====================== */

	bool x_in, y_in, z_in;

	x_in = (fabs(bba->m_max[0]) <= fabs(bbb->m_min[0]) || fabs(bbb->m_max[0]) <= fabs(bba->m_min[0]));
	y_in = (fabs(bba->m_max[1]) <= fabs(bbb->m_min[1]) || fabs(bbb->m_max[1]) <= fabs(bba->m_min[1]));
	z_in = (fabs(bba->m_max[2]) <= fabs(bbb->m_min[2]) || fabs(bbb->m_max[2]) <= fabs(bba->m_min[2]));

	if (x_in || y_in || z_in)
	{
		return IREJECT;
	}
	else
	{
		return IINTERSECT;
	}

	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO: test if a BBox and a plane intersect.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int BBoxPlaneIntersect(const BBox *theBBox, Plane *thePlane)
{
	/* =================== PUT YOUR CODE HERE ====================== */
	int min_side, max_side;
	Vector3 min, max;
	min = theBBox->m_min;
	max = theBBox->m_max;
	min_side = thePlane->whichSide(min);
	max_side = thePlane->whichSide(max);
	Vector3 points[8]{
		min,
		Vector3(max[0], min[1], min[2]),
		Vector3(min[0], max[1], min[2]),
		Vector3(min[0], min[1], max[2]),
		max,
		Vector3(min[0], max[1], max[2]),
		Vector3(max[0], min[1], max[2]),
		Vector3(max[0], max[1], min[2])};
	if (min_side != max_side)
	{
		return IINTERSECT;
	}
	else
	{
		if (min_side == 0 && max_side == 0)
		{
			return IINTERSECT;
		}
		else
		{
			int prev_side = min_side;
			for (int p = 0; p < 8; p++)
			{
				if (thePlane->whichSide(points[p]) != prev_side)
				{
					return IINTERSECT;
				}
			}
		}
		if (min_side == 1)
		{
			return +IREJECT;
		}
		else if (max_side == -1)
		{
			return -IREJECT;
		}
	}
	// Vector3 min, max, normal, center, extent;
	// float bb_range, c_dist;
	// min = theBBox->m_min;
	// max = theBBox->m_max;
	// normal = thePlane->m_n;

	// center = operator*(operator+(max,min),0.5); // find the center of bb
	// extent = operator-(max,center); // positive extent
	// // range of the bb vertices projected onto the normal that goes through the center
	// bb_range = extent[0]*fabs(normal[0]) + extent[1]*fabs(normal[1]) + extent[2]*fabs(normal[2]);
	// c_dist = thePlane->distance(center); // distance from center to plane

	// if(fabs(c_dist) <= bb_range)
	// {return IINTERSECT;}
	// else if(thePlane->whichSide(min) == 1)
	// {return +IREJECT;}
	// else if (thePlane->whichSide(max) == -1)
	// {return -IREJECT;}
	/* =================== END YOUR CODE HERE ====================== */
}

// Test if two BSpheres intersect.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBSphereIntersect(const BSphere *bsa, const BSphere *bsb)
{

	Vector3 v;
	v = bsa->m_centre - bsb->m_centre;
	float ls = v.dot(v);
	float rs = bsa->m_radius + bsb->m_radius;
	if (ls > (rs * rs))
		return IREJECT; // Disjoint
	return IINTERSECT;	// Intersect
}

// Test if a BSpheres intersect a BBox.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBBoxIntersect(const BSphere *sphere, const BBox *box)
{

	float d;
	float aux;
	float r;

	r = sphere->m_radius;
	d = 0;

	aux = sphere->m_centre[0] - box->m_min[0];
	if (aux < 0)
	{
		if (aux < -r)
			return IREJECT;
		d += aux * aux;
	}
	else
	{
		aux = sphere->m_centre[0] - box->m_max[0];
		if (aux > 0)
		{
			if (aux > r)
				return IREJECT;
			d += aux * aux;
		}
	}

	aux = (sphere->m_centre[1] - box->m_min[1]);
	if (aux < 0)
	{
		if (aux < -r)
			return IREJECT;
		d += aux * aux;
	}
	else
	{
		aux = sphere->m_centre[1] - box->m_max[1];
		if (aux > 0)
		{
			if (aux > r)
				return IREJECT;
			d += aux * aux;
		}
	}

	aux = sphere->m_centre[2] - box->m_min[2];
	if (aux < 0)
	{
		if (aux < -r)
			return IREJECT;
		d += aux * aux;
	}
	else
	{
		aux = sphere->m_centre[2] - box->m_max[2];
		if (aux > 0)
		{
			if (aux > r)
				return IREJECT;
			d += aux * aux;
		}
	}
	if (d > r * r)
		return IREJECT;
	return IINTERSECT;
}

// Test if a Triangle intersects a ray.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int IntersectTriangleRay(const Vector3 &P0,
						 const Vector3 &P1,
						 const Vector3 &P2,
						 const Line *l,
						 Vector3 &uvw)
{
	Vector3 e1(P1 - P0);
	Vector3 e2(P2 - P0);
	Vector3 p(crossVectors(l->m_d, e2));
	float a = e1.dot(p);
	if (fabs(a) < Constants::distance_epsilon)
		return IREJECT;
	float f = 1.0f / a;
	// s = l->o - P0
	Vector3 s(l->m_O - P0);
	float lu = f * s.dot(p);
	if (lu < 0.0 || lu > 1.0)
		return IREJECT;
	Vector3 q(crossVectors(s, e1));
	float lv = f * q.dot(l->m_d);
	if (lv < 0.0 || lv > 1.0)
		return IREJECT;
	uvw[0] = lu;
	uvw[1] = lv;
	uvw[2] = f * e2.dot(q);
	return IINTERSECT;
}

/* IREJECT 1 */
/* IINTERSECT 0 */

const char *intersect_string(int intersect)
{

	static const char *iint = "IINTERSECT";
	static const char *prej = "IREJECT";
	static const char *mrej = "-IREJECT";
	static const char *error = "IERROR";

	const char *result = error;

	switch (intersect)
	{
	case IINTERSECT:
		result = iint;
		break;
	case +IREJECT:
		result = prej;
		break;
	case -IREJECT:
		result = mrej;
		break;
	}
	return result;
}
