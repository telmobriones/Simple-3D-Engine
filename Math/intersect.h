// -*-C++-*-

#pragma once

#include "vector3.h"
#include "plane.h"
#include "bbox.h"
#include "bsphere.h"
#include "line.h"

#define IREJECT 1
#define IINTERSECT 0

/**
 * Check whether two BBoxes intersect
 *
 * @param bba: const BBox
 * @param bbb: const BBox
 *
 * @return IINTERSECT: intersects, IREJECT: don't intersect
 */

int BBoxBBoxIntersect(const BBox *bba, const BBox *bbb );

/**
 * Check whether a BBox intersects a given plane
 *
 * @param theBBox pointer to BBox (can't be 0)
 * @param thePlane pointer to a plane (can't be 0)
 *
 * @return  IINTERSECT  plane intersects box
 +IREJECT box is outside the plane
 -IREJECT box is inside the plane
*/

int BBoxPlaneIntersect(const BBox *theBBox, Plane *thePlane);

/**
 * Check whether two BSpheres intersect
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBSphereB pointer to Bsphere
 *
 * @return IINTERSECT: intersect; IREJECT: don't intersect
 */
int BSphereBSphereIntersect(const BSphere *oneBSphereA, const BSphere *oneBSphereB );

/**
 * Check whether a BSphere intersects with a plane
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBBox pointer to plane
 *
 * @return  IINTERSECT  plane intersects bsphere
 +IREJECT bsphere is outside the plane
 -IREJECT bsphere is inside the plane
*/
int BSpherePlaneIntersect(const BSphere *oneBSphere, Plane *oneBBox );

/**
 * Check whether a BSphere intersects with a BBox
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBBox pointer to BBox
 *
 * @return IINTERSECT: intersect; IREJECT: don't intersect
 */
int BSphereBBoxIntersect(const BSphere *oneBSphere, const BBox *oneBBox );

/**
 * Find whether a line (ray) intersects with a triangle defined by three points (P0, P1, P2).
 *
 * returns: IREJECT: don't intersect; IINTERSECT: intersect; If IINTERSECT, uvw
 *  contain the baricentric coordinates and the parameter of the line for the
 *  intersection point. That is, the intersecting point P_intersect is:
 *
 *         P_intersect = (1 - uvw[0] - uvw[1])P0 + uvw[0]P1 + uvw[1]P2
 *         P_intersect = l->m_O + uvw[2] * l->m_d
 */

int IntersectTriangleRay(const Vector3 & P0,
						 const Vector3 & P1,
						 const Vector3 & P2,
						 const Line *l,
						 Vector3 & uvw);

const char *intersect_string(int intersect);
