#ifndef RENDERER_H
#define RENDERER_H

#include "../Glm2/mat4.h"
#include "Shader/shader.h"
#include "Texture/texture_handler.h"
#include "../Camera/camera.h"

#include <glad/glad.h>

#define DEFAULT_SHADER 0

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
shader_t renderer_getShaderGLId(shader_id shader);
void renderer_setRenderMode(GLenum drawMode);
void renderer_setBlending(int useBlending);
void renderer_renderObject(struct Renderable renderable, struct Mat4 model);

void renderer_setCamera(struct Camera* cum);

/*
vData: the float array of data
vCount: the number of floats in vData
iData: the unsigned int array of indices, it is NULL if no ebo is used
iCount: if iData!=NULL then the number of indices, else the number of vertices
*/
Renderable renderer_createRenderable(const float* vData, unsigned int vCount, const unsigned int* iData, unsigned int iCount, int isFrequentlyUpdated);
void renderer_destroyRenderable(Renderable renderable);

texture_t renderer_createTexture(const char* imagePath, int channels);
void renderer_destroyTexture(texture_t texture);
void renderer_setTexture(Renderable* renderable, texture_t texture);

//doesn't change the size of the buffer (the count is the number of floats in vData)
void renderer_updateGeometry(Renderable* renderable, const float* vData, int count);

#endif
