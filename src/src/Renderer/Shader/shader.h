#ifndef SHADER_H
#define SHADER_H

#define SHADER_MAX_LENGTH 10000

#include <glad/glad.h>

#include "../Glm2/vec3.h"
#include "../Glm2/mat4.h"

typedef GLuint shader_t;

shader_t shader_import(const char* vertex, const char* fragment, const char* geometry);
void shader_delete(shader_t shader);

void shader_use(shader_t shader);
void shader_setInt(shader_t shader, const char* uniformName, int uniform);
void shader_setFloat(shader_t shader, const char* uniformName, float uniform);
void shader_setVec3(shader_t shader, const char* uniformName, float x, float y, float z);
void shader_setVec3v(shader_t shader, const char* uniformName, struct Vec3 vec);
void shader_setMat4(shader_t shader, const char* uniformName, struct Mat4 mat);

#endif
