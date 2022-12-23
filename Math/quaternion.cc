#include "quaternion.h"
#include "tools.h"
#include <stddef.h>

// cutoff for sine near zero
static float ms_fEpsilon = 1e-03;

void DestroyQuaternion(Quaternion **theQuat) {
	if (theQuat == NULL) return;
	Quaternion *thisQuat = *theQuat;
	if (!thisQuat) return;
	free (thisQuat);
	*theQuat = NULL;
}

Quaternion *CreateQuaternion(float x, float y, float z, float w) {
	Quaternion *newQuat;

	newQuat = malloc( sizeof(*newQuat) * 1 );
	newQuat->w = w;
	newQuat->x = x;
	newQuat->y = y;
	newQuat->z = z;

	return newQuat;
}

Quaternion *CreateQuaternionCopy(Quaternion *theQuat) {
	return CreateQuaternion(theQuat->x, theQuat->y, theQuat->z, theQuat->w);
}

// K matrix (from Ogre)
//
// | 00-r1X 01-r2X 02-r3X |
// | 10-r1Y 11-r2Y 12-r3Y |
// | 20-r1Y 21-r2Y 22-r3Y |

#define T00 (T->r1X)
#define T01 (T->r2X)
#define T02 (T->r3X)

#define T10 (T->r1Y)
#define T11 (T->r2Y)
#define T12 (T->r3Y)

#define T20 (T->r1Z)
#define T21 (T->r2Z)
#define T22 (T->r3Z)

Quaternion *CreateQuaternionFromTrfm3D(trfm3D *T) {

	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	Quaternion *Q;
	float fTrace;
	float fRoot;
	size_t s_iNext[3] = { 1, 2, 0 };
	float *apkQuat[3];
	float k_ii, k_jj, k_kk;
	float k_kj, k_jk, k_ij, k_ji, k_ik, k_ki;
	int   i, j, k;

	Q = CreateQuaternion(0, 0, 0, 1);

	apkQuat[0] = &Q->x;
	apkQuat[1] = &Q->y;
	apkQuat[2] = &Q->z;

	fTrace = T00 + T11 + T22;
	if (fTrace > 0.0) {
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrtf(fTrace + 1.0f);  // 2w
		Q->w = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;  // 1/(4w)
		Q->x = (T21 - T12) * fRoot;
		Q->y = (T02 - T20) * fRoot;
		Q->z = (T10 - T01) * fRoot;
	} else {
		// i = 0; j = 1; k = 2;
		i = 0;
		k_ii = T00; k_jj = T11; k_kk = T22;
		k_ij = T01; k_ji = T10;
		k_ik = T02; k_ki = T20;
		k_jk = T12; k_kj = T21;
		if (T11 > T00) {
			// i = 1; j = 2; k = 0;
			i = 1;
			k_ii = T11; k_jj = T22; k_kk = T00;
			k_ij = T12; k_ji = T21;
			k_ik = T10; k_ki = T01;
			k_jk = T20; k_kj = T02;
		}
		if (T22 > k_ii) {
			// i = 2; j = 0; k = 1;
			i = 2;
			k_ii = T22; k_jj = T00; k_kk = T11;
			k_ij = T20; k_ji = T02;
			k_ik = T21; k_ki = T12;
			k_jk = T01; k_kj = T10;
		}
		j = s_iNext[i];
		k = s_iNext[j];
		fRoot = sqrtf(k_ii - k_jj - k_kk + 1.0f);
		*apkQuat[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		Q->w = (k_kj - k_jk) * fRoot;
		*apkQuat[j] = (k_ji + k_ij) * fRoot;
		*apkQuat[k] = (k_ki + k_ik) * fRoot;
	}
	return Q;
}

void QuaternionToTrfm3D(const Quaternion *Q, trfm3D *T) {

	float fTx  = Q->x + Q->x;
	float fTy  = Q->y + Q->y;
	float fTz  = Q->z + Q->z;
	float fTwx = fTx * Q->w;
	float fTwy = fTy * Q->w;
	float fTwz = fTz * Q->w;
	float fTxx = fTx * Q->x;
	float fTxy = fTy * Q->x;
	float fTxz = fTz * Q->x;
	float fTyy = fTy * Q->y;
	float fTyz = fTz * Q->y;
	float fTzz = fTz * Q->z;

	T00 = 1.0f - (fTyy + fTzz);
	T01 = fTxy - fTwz;
	T02 = fTxz + fTwy;
	T10 = fTxy + fTwz;
	T11 = 1.0f - (fTxx + fTzz);
	T12 = fTyz - fTwx;
	T20 = fTxz - fTwy;
	T21 = fTyz + fTwx;
	T22 = 1.0f - (fTxx + fTyy);
}

#undef T00
#undef T01
#undef T02
#undef T10
#undef T11
#undef T12
#undef T20
#undef T21
#undef T22

// tgt = src + tgt
void AddQuaternion(const Quaternion *src, Quaternion *tgt) {
	tgt->x += src->x;
	tgt->y += src->y;
	tgt->z += src->z;
	tgt->w += src->w;
}

// tgt = src - tgt
void SubstractQuaternion(const Quaternion *src, Quaternion *tgt) {
	tgt->x -= src->x;
	tgt->y -= src->y;
	tgt->z -= src->z;
	tgt->w -= src->w;
}

void  QuaternionScalarProduct(Quaternion *tgt, float scalar) {
	tgt->x *= scalar;
	tgt->y *= scalar;
	tgt->z *= scalar;
	tgt->w *= scalar;
}

// tgt = src * tgt
void  QuaternionProduct(const Quaternion *src, Quaternion *tgt) {

	// NOTE:  Multiplication is not generally commutative, so in most
	// cases src*tgt != tgt*src

	float x, y, z, w;

	w = src->w * tgt->w - src->x * tgt->x - src->y * tgt->y - src->z * tgt->z;
	x = src->w * tgt->x + src->x * tgt->w + src->y * tgt->z - src->z * tgt->y;
	y = src->w * tgt->y + src->y * tgt->w + src->z * tgt->x - src->x * tgt->z;
	z = src->w * tgt->z + src->z * tgt->w + src->x * tgt->y - src->y * tgt->x;

	tgt->x = x;
	tgt->y = y;
	tgt->z = z;
	tgt->w = w;
}

float QuaternionDot(const Quaternion *A, const Quaternion *B) {

	return A->x * B->x +
		A->y * B->y +
		A->z + B->z +
		A->w + B->w;
}

float QuaternionNorm(const Quaternion *A) {

	return QuaternionDot(A, A);
}

/// Normalizes this quaternion, and returns the previous length
float QuaternionNormalize(Quaternion *theQuat) {

	float len, factor;

	len = QuaternionNorm(theQuat);
	factor = 1.0f / sqrtf(len);
	QuaternionScalarProduct(theQuat, factor);

	return len;
}

// tgt = src^{-1}
// note: apply to non-zero quaternion
void QuaternionInverse(const Quaternion *src, Quaternion *tgt) {

	float fNorm, fInvNorm;

	fNorm = QuaternionNorm(src);
	if ( fNorm > 0.0 ) {
		fInvNorm = 1.0f / fNorm;
		tgt->x = -src->x * fInvNorm;
		tgt->y = -src->y * fInvNorm;
		tgt->z = -src->z * fInvNorm;
		tgt->w =  src->w * fInvNorm;
	} else {
		// return an invalid result to flag the error
		tgt->x = 0;
		tgt->y = 0;
		tgt->z = 0;
		tgt->w = 0;
	}
}

// tgt = src^{-1}
// note: apply to unit-lenght quaternion
void QuaternionUnitInverse(const Quaternion *src, Quaternion *tgt) {
	tgt->x = -src->x;
	tgt->y = -src->y;
	tgt->z = -src->z;
	tgt->w =  src->w;
}

void QuaternionExp(const Quaternion *src, Quaternion *tgt) {

	// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
	// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

	float fAngle;
	float fSin;
	float fCoeff;

	fAngle = sqrtf(src->x * src->x + src->y * src->y + src->z * src->z); // Angle in radians
	fSin = sinf(fAngle);
	tgt->w = cosf(fAngle);

	if (fabsf(fSin) >= ms_fEpsilon) {
		fCoeff = fSin / fAngle;
		tgt->x = fCoeff * src->x;
		tgt->y = fCoeff * src->y;
		tgt->z = fCoeff * src->z;
	} else {
		tgt->x = src->x;
		tgt->y = src->y;
		tgt->z = src->z;
	}
}

void QuaternionLog(const Quaternion *src, Quaternion *tgt) {

	// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
	// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

	float fAngle;
	float fSin;
	float fCoeff;

	tgt->w = 0.0;
	if ( fabsf(src->w) < 1.0f ) {
		fAngle = acosf(src->w); // In radians
		fSin = sinf(fAngle);
		if (fabsf(fSin) >= ms_fEpsilon ) {
			fCoeff = fAngle / fSin;
			tgt->x = fCoeff * src->x;
			tgt->y = fCoeff * src->y;
			tgt->z = fCoeff * src->z;
			return;
		}
	}
	tgt->x = src->x;
	tgt->y = src->y;
	tgt->z = src->z;
}

// rotation of a vector by a quaternion
void QuaternionTransformVector(const Quaternion *src , float *vx, float *vy, float *vz ) {

	// nVidia SDK implementation

	float uv[3], uuv[3];
	float qvec[3] = { src->x, src->y, src->z };
	float fFactor;

	// uv = qvec * v
	crossVV(&uv[0], &uv[1], &uv[2],
			qvec[0], qvec[1], qvec[2],
			*vx, *vy, *vz);
	// uuv = qvec * uv
	crossVV(&uuv[0], &uuv[1], &uuv[2],
			qvec[0], qvec[1], qvec[2],
			uv[0], uv[1], uv[2]);

	fFactor = 2.0f * src->w;
	uv[0] *= fFactor; uv[1] *= fFactor; uv[2] *= fFactor;
	uuv[0] *= 2.0f; uuv[1] *= 2.0f; uuv[2] *= 2.0f;

	*vx += uv[0] + uuv[0];
	*vy += uv[1] + uuv[1];
	*vz += uv[2] + uuv[2];
}

float QuaternionGetRoll(const Quaternion *src, int reprojectAxis) {

	//float fTx;
	float fTy;
	float fTz;
	float fTwz;
	float fTxy;
	float fTyy;
	float fTzz;

	float res; // in radians

	if (reprojectAxis) {
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need

		//fTx  = 2.0 * src->x;
		fTy  = 2.0f * src->y;
		fTz  = 2.0f * src->z;
		fTwz = fTz * src->w;
		fTxy = fTy * src->x;
		fTyy = fTy * src->y;
		fTzz = fTz * src->z;

		// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
		res = atan2f(fTxy + fTwz, 1.0f - (fTyy + fTzz));
	} else {
		res = atan2f(2*(src->x * src->y + src->w * src->z),
					 src->w * src->w + src->x * src->x - src->y * src->y - src->z * src->z);
	}
	return res;
}

float QuaternionGetPitch(const Quaternion *src, int reprojectAxis) {

	float fTx;
	//float fTy;
	float fTz;
	float fTwx;
	float fTxx;
	float fTyz;
	float fTzz;

	float res; // in radians

	if (reprojectAxis) {
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need

		fTx  = 2.0 * src->x;
		//fTy  = 2.0f * src->y;
		fTz  = 2.0f * src->z;
		fTwx = fTx * src->w;
		fTxx = fTx * src->x;
		fTyz = fTz * src->y;
		fTzz = fTz * src->z;

		// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
		res = atan2f(fTyz + fTwx, 1.0f - (fTxx + fTzz));
	} else {
		res = atan2f(2*(src->y * src->z + src->w*src->x),
					 src->w*src->w - src->x*src->x - src->y*src->y + src->z*src->z);
	}
	return res;
}

float QuaternionGetYaw(const Quaternion *src, int reprojectAxis) {

	float fTx;
	float fTy;
	float fTz;
	float fTwy;
	float fTxx;
	float fTxz;
	float fTyy;

	float res; // in radians

	if (reprojectAxis) {
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need

		fTx  = 2.0f * src->x;
		fTy  = 2.0f * src->y;
		fTz  = 2.0f * src->z;
		fTwy = fTy * src->w;
		fTxx = fTx * src->x;
		fTxz = fTz * src->x;
		fTyy = fTy * src->y;

		// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));
		res = atan2f(fTxz + fTwy, 1.0f - (fTxx + fTyy));
	} else {
		// internal version
		res = asinf(-2*(src->x*src->z - src->w*src->y));
	}
	return res;
}

/** Performs Spherical linear interpolation between two quaternions, and returns the result.
	B = Slerp( fT, A, B). Take into account that:
	Slerp ( 0.0f, A, B ) = A
	Slerp ( 1.0f, A, B ) = B
	@remarks
	Slerp has the proprieties of performing the interpolation at constant
	velocity, and being torque-minimal (unless shortestPath=false).
	However, it's NOT commutative, which means
	Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
	therefore be careful if your code relies in the order of the operands.
	This is specially important in IK animation.
*/

void QuaternionSlerp(const Quaternion *rkP, const Quaternion *rkQ,
					 Quaternion *tgt, float fT, int shortestPath) {

	float fSin, fInvSin;
	float fAngle;
	float fCoeff0, fCoeff1;
	float fCos;
	float x, y, z, w;

	x = y = z = 0.0f;
	w = 1.0f;

	fCos = QuaternionDot(rkP, rkQ);

	if (fCos < 0.0f && shortestPath) {
		fCos = -fCos;
		x = -rkQ->x;
		y = -rkQ->y;
		z = -rkQ->z;
		w = -rkQ->w;
	} else {
		x = rkQ->x;
		y = rkQ->y;
		z = rkQ->z;
		w = rkQ->w;
	}

	if (fabsf(fCos) < 1 - ms_fEpsilon) {
		// Standard case (slerp)
		fSin = sqrtf(1 - sqrtf(fCos));
		fAngle = atan2f(fSin, fCos);
		fInvSin = 1.0f / fSin;
		fCoeff0 = sinf((1.0f - fT) * fAngle) * fInvSin;
		fCoeff1 = sinf(fT * fAngle) * fInvSin;
		tgt->x = fCoeff0 * rkP->x + fCoeff1 * x;
		tgt->y = fCoeff0 * rkP->y + fCoeff1 * y;
		tgt->z = fCoeff0 * rkP->z + fCoeff1 * z;
		tgt->w = fCoeff0 * rkP->w + fCoeff1 * w;
		return;
	} else {
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		tgt->x = (1.0f - fT) * rkP->x + fT * x;
		tgt->y = (1.0f - fT) * rkP->y + fT * x;
		tgt->z = (1.0f - fT) * rkP->z + fT * z;
		tgt->w = (1.0f - fT) * rkP->w + fT * w;
		QuaternionNormalize(tgt);
		return;
	}
}

/** @See Slerp. It adds extra "spins" (i.e. rotates several times) specified
	by parameter 'iExtraSpins' while interpolating before arriving to the
	final values
*/
void QuaternionSlerpExtraSpins(const Quaternion *rkP, const Quaternion *rkQ,
							   Quaternion *tgt, float fT, int iExtraSpins) {

	float fSin, fInvSin;
	float fPhase;
	float fCoeff0, fCoeff1;
	float fCos;
	float fAngle;

	fCos = QuaternionDot(rkP, rkQ);
	fAngle = acosf(fCos);

	if (fabsf(fAngle) < ms_fEpsilon ) {
		tgt->x = rkP->x;
		tgt->y = rkP->y;
		tgt->z = rkP->z;
		tgt->w = rkP->w;
		return;
	}

	fSin = sinf(fAngle);
	fPhase = PI_VALUE * iExtraSpins * fT;
	fInvSin = 1.0f/fSin;
	fCoeff0 = sinf((1.0f - fT) * fAngle - fPhase) * fInvSin;
	fCoeff1 = sinf(fT * fAngle + fPhase) * fInvSin;
	tgt->x = fCoeff0 * rkP->x + fCoeff1 * rkQ->x;
	tgt->y = fCoeff0 * rkP->y + fCoeff1 * rkQ->y;
	tgt->z = fCoeff0 * rkP->z + fCoeff1 * rkQ->z;
	tgt->w = fCoeff0 * rkP->w + fCoeff1 * rkQ->w;
	return;
}

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
void QuaternionNlerp(const Quaternion *rkP, const Quaternion *rkQ,
					 Quaternion *tgt, float fT, int shortestPath) {

	float fCos;
	fCos= QuaternionDot(rkP, rkQ);
	if (fCos < 0.0f && shortestPath) {
		tgt->x = rkP->x + fT * ((-rkQ->x) - rkP->x);
		tgt->y = rkP->y + fT * ((-rkQ->y) - rkP->y);
		tgt->z = rkP->z + fT * ((-rkQ->z) - rkP->z);
		tgt->w = rkP->w + fT * ((-rkQ->w) - rkP->w);
	}	else {
		tgt->x = rkP->x + fT * (rkQ->x - rkP->x);
		tgt->y = rkP->y + fT * (rkQ->y - rkP->y);
		tgt->z = rkP->z + fT * (rkQ->z - rkP->z);
		tgt->w = rkP->w + fT * (rkQ->w - rkP->w);
	}
	QuaternionNormalize(tgt);
	return;
}

void PrintQuaternion(Quaternion *b) {
	printf("Quaternion(w:%.2f, x:%.2f, y:%.2f, z:%.2f) ", b->w, b->x, b->y, b->z);
}
