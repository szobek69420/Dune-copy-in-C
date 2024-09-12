#ifndef RENDERER_H
#define RENDERER_H

#include "../Glm2/mat4.h"
#include "Shader/shader.h"
#include "../Camera/camera.h"

#include <glad/glad.h>

struct Renderable {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	unsigned int count;//vertex or index count depending on if the ebo is used or not
};
typedef struct Renderable Renderable;

typedef unsigned int shader_id;

void renderer_init();
void renderer_deinit();

shader_id renderer_createShader(const char* vs, const char* fs, const char* gs);//gives back the id with which the shader can be accessed
void renderer_useShader(shader_id shader);
void renderer_setRenderConfig(int eboUsed, GLenum drawMode);
void renderer_renderObject(struct Renderable renderable, struct Mat4 model, texture kell);

void renderer_setCamera(struct Camera* cum);

#endif
