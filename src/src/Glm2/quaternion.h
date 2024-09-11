#ifndef QUATERNION_H
#define QUATERNION_H

#include "vec3.h"
#include "vec4.h"
#include "mat4.h"

struct Quaternion {
	float s, x, y, z;
};

typedef struct Quaternion Quaternion;
typedef struct Quaternion Quat;

struct Quaternion quat_init();
struct Quaternion quat_initValues(float s, float x, float y, float z);
struct Quaternion quat_initVector(struct Vec4 vec);
struct Quaternion quat_initRotation(float angleInRads, struct Vec3 axis);

struct Quaternion quat_scale(struct Quaternion quat, float scale);
struct Quaternion quat_sum(struct Quaternion a, struct Quaternion b);
struct Quaternion quat_multiply(struct Quaternion a, struct Quaternion b);

float quat_angle(struct Quaternion quat);
struct Vec3 quat_axis(struct Quaternion quat);
float quat_magnitude(struct Quaternion quat);

struct Mat4 quat_rotationMatrix(struct Quaternion quat);

struct Quaternion quat_normalize(struct Quaternion quat);
struct Quaternion quat_inverse(struct Quaternion quat);
struct Quaternion quat_lerp(struct Quaternion a, struct Quaternion b, float i);

struct Vec3 quat_rotateVector(struct Quaternion quat, struct Vec3 vec);

#endif