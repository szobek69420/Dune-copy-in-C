#ifndef MAT3_H
#define MAT3_H

#include "vec3.h"
#include "mat4.h"

struct Mat4;

typedef struct Mat3{
	float data[9];
} Mat3; //column-major order

struct Mat3 mat3_create(float szam);
struct Mat3 mat3_create2(float* values);
struct Mat3 mat3_createFromVec(struct Vec3 col0, struct Vec3 col1, struct Vec3 col2);
struct Mat3 mat3_createFromMat(struct Mat4);

void mat3_set(struct Mat3* mat, int row, int col, float value);

struct Mat3 mat3_transpose(struct Mat3 mat);

struct Mat3 mat3_sum(struct Mat3 egy, struct Mat3 ketto);
struct Mat3 mat3_subtract(struct Mat3 egy, struct Mat3 ketto);
struct Mat3 mat3_multiply(struct Mat3 egy, struct Mat3 ketto);

float mat3_determinant(struct Mat3* mat);

struct Mat3 mat3_inverse(struct Mat3 mat);

void mat3_print(struct Mat3* mat);

#endif