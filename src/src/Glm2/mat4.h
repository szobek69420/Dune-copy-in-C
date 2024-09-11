#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include "vec4.h"
#include "mat3.h"


typedef struct Mat4 {
	float data[16];
} Mat4; //column-major order

struct Mat4 mat4_create(float szam);
struct Mat4 mat4_create2(float* data);
struct Mat4 mat4_createFromVec(struct Vec4 col0, struct Vec4 col1, struct Vec4 col2, struct Vec4 col3);
struct Mat4 mat4_createFromMat(struct Mat3);

float mat4_get(struct Mat4* mat, int row, int col, float value);
void  mat4_set(struct Mat4* mat, int row, int col, float value);

float mat4_determinant(struct Mat4* mat);

struct Mat4 mat4_sum(struct Mat4 egy, struct Mat4 katto);
struct Mat4 mat4_multiply(struct Mat4 egy, struct Mat4 katto);

struct Mat4 mat4_transpose(struct Mat4 mat);
struct Mat4 mat4_inverse(struct Mat4 mat);


struct Mat4 mat4_scale(struct Mat4 mat, struct Vec3 scale);
struct Mat4 mat4_rotate(struct Mat4 mat, struct Vec3 axis, float angle);
struct Mat4 mat4_translate(struct Mat4 mat, struct Vec3 pos);


struct Mat4 mat4_lookAt(struct Vec3 pos, struct Vec3 direction, struct Vec3 up);

struct Mat4 mat4_perspective(float fov, float aspectXY, float near, float far);
struct Mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far);

void mat4_print(struct Mat4* mat);

#endif