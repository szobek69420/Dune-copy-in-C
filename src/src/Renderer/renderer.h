#ifndef RENDERER_H
#define RENDERER_H

#include "../Glm2/mat4.h"
#include "Shader/shader.h"
#include "Texture/texture_handler.h"
#include "../Camera/camera.h"

#include <glad/glad.h>

struct Renderable {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	unsigned int count;//vertex or index count depending on if the ebo is used or not
	int eboUsed;

	texture_t texture;
};
typedef struct Renderable Renderable;

typedef unsigned int shader_id;

void renderer_init();
void renderer_deinit();

shader_id renderer_createShader(const char* vs, const char* fs, const char* gs);//gives back the id with which the shader can be accessed, gs can be NULL
void renderer_useShader(shader_id shader);
void renderer_destroyShader(shader_id shader);
void renderer_setRenderMode(GLenum drawMode);
void renderer_renderObject(struct Renderable renderable, struct Mat4 model);

void renderer_setCamera(struct Camera* cum);

Renderable renderer_createRenderable(const float* vData, unsigned int vCount, const unsigned int* iData, unsigned int iCount);//iData should be NULL if no ebo is used
void renderer_destroyRenderable(Renderable renderable);

texture_t renderer_createTexture(const char* imagePath, int channels);
void renderer_setTexture(Renderable* renderable, texture_t texture);

#endif
