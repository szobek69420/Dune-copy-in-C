#ifndef VEC4_H
#define VEC4_H

struct Mat4;

typedef struct Vec4{
	float x;
	float y;
	float z;
    	float w;
} Vec4;

struct Vec4 vec4_create(float);
struct Vec4 vec4_create2(float, float, float,float);

struct Vec4 vec4_sum(struct Vec4, struct Vec4);
struct Vec4 vec4_subtract(struct Vec4, struct Vec4);
float vec4_dot(struct Vec4, struct Vec44);

float vec4_sqrMagnitude(struct Vec4);
float vec4_magnitude(struct Vec4);
struct Vec4 vec4_normalize(struct Vec4);
struct Vec4 vec4_scale(struct Vec4, float);

struct Vec4 vec4_multiplyWithMatrix(struct Mat4, struct Vec4);

void vec4_print(struct Vec4*);

#endif