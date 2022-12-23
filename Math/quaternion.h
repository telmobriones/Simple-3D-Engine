// -*-C++-*-

#pragma once

/**
 * @file   quaternion.h
 * @author a.soroa <ccpsoeta@sipc87.si.ehu.es>
 * @date   Tue Jan 31 10:25:32 2012
 *
 * @brief  Implementation of a Quaternion, i.e. a rotation around an axis.
 *         The code is borrowed from Ogre.
 *		   For more information about Quaternions and the theory behind it, we recomend reading:
 *		   http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer
 *		   http://www.cprogramming.com/tutorial/3d/quaternions.html
 *		   http://www.gamedev.net/page/resources/_/reference/programming/math-and-physics/quaternions/quaternion-powers-r1095
 *
 *
 */


#include "trfm3D.h"

typedef struct _Quaternion {
	float w, x, y, z;
} Quaternion;

void DestroyQuaternion(Quaternion **theQuat);

Quaternion *CreateQuaternion(float x, float y, float z, float w);
Quaternion *CreateQuaternionFromTrfm3D(trfm3D *T);
Quaternion *CreateQuaternionCopy(Quaternion *theQuat);

void QuaternionToTrfm3D(const Quaternion *theQuat, trfm3D *T);

// tgt = src + tgt
void AddQuaternion(const Quaternion *src, Quaternion *tgt);

// tgt = src - tgt
void SubstractQuaternion(const Quaternion *src, Quaternion *tgt);

void  QuaternionScalarProduct(Quaternion *src, float scalar);

// tgt = src * tgt
void  QuaternionProduct(const Quaternion *src, Quaternion *tgt);

float QuaternionDot(const Quaternion *theQuat, const Quaternion *otherQuat);
float QuaternionNorm(const Quaternion *theQuat);

/// Normalizes this quaternion, and returns the previous length
float QuaternionNormalize(Quaternion *theQuat);

// tgt = src^{-1}
// note: apply to non-zero quaternion
void QuaternionInverse(const Quaternion *src, Quaternion *tgt);

// tgt = src^{-1}
// note: apply to unit-lenght quaternion
void QuaternionUnitInverse(const Quaternion *src, Quaternion *tgt);

void QuaternionExp(const Quaternion *src, Quaternion *tgt);
void QuaternionLog(const Quaternion *src, Quaternion *tgt);

// rotation of a vector by a quaternion
void QuaternionTransformVector(const Quaternion *src , float *xtr, float *ytr, float *ztr );

/**
 * Calculate the local roll element of this quaternion.
 *
 * @param src the quaternion
 * @param reprojectAxis By default the method returns the 'intuitive' result
 that is, if you projected the local Y of the quaternion onto the X and
 Y axes, the angle between them is returned. If set to false though, the
 result is the actual yaw that will be used to implement the quaternion,
 which is the shortest possible path to get to the same orientation and
 may involve less axial rotation.

 @return: roll angle in degrees
**/

float QuaternionGetRoll(const Quaternion *src, int reprojectAxis);


/**
 * Calculate the local pitch element of this quaternion
 *
 * @param src the Quaternion
 @param reprojectAxis By default the method returns the 'intuitive' result
 that is, if you projected the local Z of the quaternion onto the X and
 Y axes, the angle between them is returned. If set to true though, the
 result is the actual yaw that will be used to implement the quaternion,
 which is the shortest possible path to get to the same orientation and
 may involve less axial rotation.
 @return: pitch angle in degrees
*/

float QuaternionGetPitch(const Quaternion *src, int reprojectAxis);

/**
 * Calculate the local yaw element of this quaternion
 *
 * @param src the Quqternion
 @param reprojectAxis By default the method returns the 'intuitive' result
 that is, if you projected the local Z of the quaternion onto the X and
 Z axes, the angle between them is returned. If set to true though, the
 result is the actual yaw that will be used to implement the quaternion,
 which is the shortest possible path to get to the same orientation and
 may involve less axial rotation.
 @return: yaw angle in degrees
*/

float QuaternionGetYaw(const Quaternion *src, int reprojectAxis);


/**
   Performs Spherical linear interpolation between two quaternions, and returns the result.
   C = Slerp( fT, A, B).

   Take into account that:
   Slerp ( 0.0f, A, B ) = A
   Slerp ( 1.0f, A, B ) = B

   @param fT the step
   @param A source Quaternion
   @param B target Quaternion

   @return the iterpolated Quaternion

   @remarks
   Slerp has the proprieties of performing the interpolation at constant
   velocity, and being torque-minimal (unless shortestPath=false).
   However, it's NOT commutative, which means
   Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
   therefore be careful if your code relies in the order of the operands.
   This is specially important in IK animation.
*/

void QuaternionSlerp(const Quaternion *A, const Quaternion *B,
					 Quaternion *C, float fT, int shortestPath);

/** @See Slerp. It adds extra "spins" (i.e. rotates several times) specified
	by parameter 'iExtraSpins' while interpolating before arriving to the
	final values
*/
void QuaternionSlerpExtraSpins(const Quaternion *A, const Quaternion *B,
							   Quaternion *C, float fT, int iExtraSpins);

/** Performs Normalised linear interpolation between two quaternions, and returns the result.
	nlerp ( 0.0f, A, B ) = A
	nlerp ( 1.0f, A, B ) = B
	@remarks
	Nlerp is faster than Slerp.
	Nlerp has the proprieties of being commutative (@See Slerp;
	commutativity is desired in certain places, like IK animation), and
	being torque-minimal (unless shortestPath=false). However, it's performing
	the interpolation at non-constant velocity; sometimes this is desired,
	sometimes it is not. Having a non-constant velocity can produce a more
	natural rotation feeling without the need of tweaking the weights; however
	if your scene relies on the timing of the rotation or assumes it will point
	at a specific angle at a specific weight value, Slerp is a better choice.
*/
void QuaternionNlerp(const Quaternion *A, const Quaternion *B,
					 Quaternion *C, float fT, int shortestPath);

void PrintQuaternion(Quaternion *theQuat);
