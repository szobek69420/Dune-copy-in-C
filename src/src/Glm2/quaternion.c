#include <math.h>
#include "quaternion.h"

#include "vec3.h"
#include "vec4.h"
#include "mat4.h"

#define RAD2DEG 0.01745329f

struct Quaternion quat_init()
{
	Quat quat;
	quat.s = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;
	return quat;
}

struct Quaternion quat_initValues(float s, float x, float y, float z)
{
	Quat quat;
	quat.s = s;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

struct Quaternion quat_initVector(struct Vec4 vec)
{
	Quat quat;
	quat.s = vec.x;
	quat.x = vec.y;
	quat.y = vec.z;
	quat.z = vec.w;
	return quat;
}

struct Quaternion quat_initRotation(float angleInRads, struct Vec3 axis)
{
	Quat quat;
	quat.s = cosf(0.5f * angleInRads);

	do {
		float temp = vec3_magnitude(axis);
		if (temp > 0.00001f)
			axis = vec3_scale(axis,1/temp);
	} while (0);

	float temp = sinf(0.5f * angleInRads);
	quat.x = temp * axis.x;
	quat.y = temp * axis.y;
	quat.z = temp * axis.z;
	return quat;
}

struct Quaternion quat_scale(struct Quaternion quat, float scale)
{
	Quat quat2;
	quat2.s = scale * quat.s;
	quat2.x = scale * quat.x;
	quat2.y = scale * quat.y;
	quat2.z = scale * quat.z;
	return quat2;
}

struct Quaternion quat_sum(struct Quaternion a, struct Quaternion b)
{
	Quat quat;
	quat.s = a.s + b.s;
	quat.x = a.x + b.x;
	quat.y = a.y + b.y;
	quat.z = a.z + b.z;
	return quat;
}

struct Quaternion quat_multiply(struct Quaternion a, struct Quaternion b)
{
	Vec3 v1 = (Vec3){ a.x,a.y,a.z };
	Vec3 v2 = (Vec3){ b.x,b.y,b.z };
	Vec3 temp = vec3_cross(v1, v2);
	temp.x += a.s * v2.x + b.s * v1.x;
	temp.y += a.s * v2.y + b.s * v1.y;
	temp.z += a.s * v2.z + b.s * v1.z;

	Quat quat;
	quat.s = a.s + b.s - vec3_dot(v1, v2);
	quat.x = temp.x;
	quat.y = temp.y;
	quat.z = temp.z;

	return quat;
}

float quat_angle(struct Quaternion quat)
{
	float cosAngle = quat.s;
	float sinAngle = vec3_magnitude((Vec3) { quat.x, quat.y, quat.z });

	float angle = 2 * atan2f(sinAngle, cosAngle);
	return angle;
}

struct Vec3 quat_axis(struct Quaternion quat)
{
	Vec3 axis = (Vec3){ quat.x,quat.y,quat.z };
	float length = vec3_magnitude(axis);
	if(length>0.00001f)
		return vec3_scale(axis, 1 / length);
	return axis;
}

float quat_magnitude(struct Quaternion quat)
{
	return sqrtf(quat.s * quat.s + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
}

struct Mat4 quat_rotationMatrix(struct Quaternion quat)
{
	Vec3 axis = quat_axis(quat);
	if (vec3_magnitude(axis) < 0.00001f)
		return mat4_create(1);

	return mat4_rotate(mat4_create(1), axis, RAD2DEG * quat_angle(quat));
}

struct Quaternion quat_normalize(struct Quaternion quat)
{
	float length = quat_magnitude(quat);
	if (length < 0.00001f)
		return quat;
	return quat_scale(quat, 1 / length);
}
struct Quaternion quat_inverse(struct Quaternion quat)
{
	float length = quat_magnitude(quat);
	if (length< 0.00001f)
		return quat;

	Quat quat2;
	quat2.s = quat.s;
	quat2.x = -quat.x;
	quat2.y = -quat.y;
	quat2.z = -quat.z;
	return quat_scale(quat2, 1/powf(length, 2.0f));
}

struct Quaternion quat_lerp(struct Quaternion a, struct Quaternion b, float i)
{
	float angle = a.s * b.s + a.x * b.x + a.y * b.y + a.z * b.z;
	angle /= quat_magnitude(a) * quat_magnitude(b);
	angle = acosf(angle);

	float denominator = sinf(angle);

	return quat_sum(
		quat_scale(a, sinf((1 - i) * angle) / denominator),
		quat_scale(b, (sinf(i * angle) / denominator))
	);
}

struct Vec3 quat_rotateVector(struct Quaternion quat, struct Vec3 vec)
{
	Quat result= quat_multiply(quat_multiply(quat, quat_initValues(0, vec.x, vec.y, vec.z)),quat_inverse(quat));
	return (Vec3) { result.x, result.y, result.z };
}