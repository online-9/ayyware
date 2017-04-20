/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Vector.h"
#include "../SDK.h"
#include <stdint.h>

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define RADPI 57.295779513082f

void AngleVectors(const Vector &angles, Vector *forward);
void VectorTransform(const Vector in1, const matrix3x4 in2, Vector &out);
void SinCos(float a, float* s, float*c);
void VectorAngles(Vector forward, Vector &angles);
void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
void Normalize(Vector &vIn, Vector &vOut);
void CalcAngle(Vector src, Vector dst, Vector &angles);

void NormaliseViewAngle(Vector &angle);
Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox);
void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,
	const IClientEntity *ignore, int collisionGroup, trace_t *ptr);
bool IsVisible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID);

inline float AngleNormalize(float angle)
{
	return (360.0 / 65536) * ((int)((angle + 180) * (65536.0 / 360.0)) & 65535) - 180;
}

void VectorAngles(const Vector &forward, const Vector &pseudoup, Vector &angles);