#ifndef VEC3_H
#define VEC3_H
typedef struct Vec3{
	float x;
	float y;
	float z;
} Vec3;

struct Vec3 vec3_create(float);
struct Vec3 vec3_create2(float, float, float);

struct Vec3 vec3_sum(struct Vec3, struct Vec3);
struct Vec3 vec3_subtract(struct Vec3, struct Vec3);
float vec3_dot(struct Vec3, struct Vec3);
struct Vec3 vec3_cross(struct Vec3, struct Vec3);

float vec3_sqrMagnitude(struct Vec3);
float vec3_magnitude(struct Vec3);
struct Vec3 vec3_normalize(struct Vec3);
struct Vec3 vec3_scale(struct Vec3, float);

struct Vec3 vec3_lerp(struct Vec3 a, struct Vec3 b, float i);

//normal must not be a null vector
struct Vec3 vec3_reflect(struct Vec3 vec, struct Vec3 normal);

void vec3_print(struct Vec3*);

#endif