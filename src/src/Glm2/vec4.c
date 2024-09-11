#include "vec4.h"
#include "mat4.h"

#include <math.h>
#include <stdio.h>

Vec4 vec4_create(float szam)
{
	Vec4 vec;
	vec.x = szam;
	vec.y = szam;
	vec.z = szam;
	vec.w = szam;
	return vec;
}
Vec4 vec4_create2(float x, float y, float z, float w)
{
	Vec4 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;
	return vec;
}

Vec4 vec4_sum(Vec4 egy, Vec4 katto)
{
	egy.x += katto.x;
	egy.y += katto.y;
	egy.z += katto.z;
	egy.w += katto.w;
	return egy;
}

Vec4 vec4_subtract(Vec4 egy, Vec4 katto)
{
	egy.x -= katto.x;
	egy.y -= katto.y;
	egy.z -= katto.z;
	egy.w -= katto.w;
	return egy;
}

float vec4_dot(Vec4 egy, Vec4 katto)
{
	float szam = egy.x * katto.x+egy.y*katto.y+egy.z*katto.z+egy.w*katto.w;
	return szam;
}

float vec4_sqrMagnitude(Vec4 vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

float vec4_magnitude(Vec4 vec)
{
	float szam = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
	szam = sqrtf(szam);
	return szam;
}

Vec4 vec4_normalize(Vec4 vec)
{
	float szam = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
	if (szam == 0)
		return vec;

	szam = 1/sqrtf(szam);
	vec.x *= szam;
	vec.y *= szam;
	vec.z *= szam;
	vec.w *= szam;

	return vec;
}

Vec4 vec4_scale(Vec4 vec, float szam)
{
	vec.x *= szam;
	vec.y *= szam;
	vec.z *= szam;
	vec.w *= szam;

	return vec;
}

Vec4 vec4_multiplyWithMatrix(Mat4 mat, Vec4 vec)
{
	Vec4 result;
	result.x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[8] * vec.z + mat.data[12] * vec.w;
	result.y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[9] * vec.z + mat.data[13] * vec.w;
	result.z = mat.data[2] * vec.x + mat.data[6] * vec.y + mat.data[10] * vec.z + mat.data[14] * vec.w;
	result.w = mat.data[3] * vec.x + mat.data[7] * vec.y + mat.data[11] * vec.z + mat.data[15] * vec.w;

	return result;
}

void vec4_print(Vec4* vec)
{
	printf("(%.2f; %.2f; %.2f; %.2f)\n", vec->x, vec->y, vec->z, vec->w);
}