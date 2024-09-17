#define _CRT_SECURE_NO_WARNINGS

#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <seqtor.h>
#include "Shader/shader.h"

struct ShaderInfo {
	shader_id id;//used by the outside world
	shader_t glId;//given by opengl
	char* vs;
	char* fs;
	char* gs;
	int referenceCount;
};
typedef struct ShaderInfo ShaderInfo;

static seqtor_of(ShaderInfo) registeredShaders;
static unsigned int currentShaderId = 0;

static GLenum drawMode = GL_TRIANGLES;

static Camera* camera=NULL;

static shader_t currentShader = 0;

void renderer_init()
{
	seqtor_init(registeredShaders, 1);
	renderer_createShader("Assets/Shaders/default.vag", "Assets/Shaders/default.fag", NULL);

	textureHandler_init();
}

void renderer_deinit()
{
	for (int i = seqtor_size(registeredShaders); i >= 0; i--)
		renderer_destroyShader(seqtor_at(registeredShaders, i).id);
	seqtor_destroy(registeredShaders);

	textureHandler_deinit();
}

shader_id renderer_createShader(const char* vs, const char* fs, const char* gs)//gives back the id with which the shader can be accessed
{
	for (int i = 0; i < seqtor_size(registeredShaders); i++)
	{
		ShaderInfo* psi = &seqtor_at(registeredShaders, i);

		if (strcmp(vs, psi->vs) != 0)
			continue;
		if (strcmp(fs, psi->fs) != 0)
			continue;
		if (gs != NULL && psi->gs == NULL|| gs == NULL && psi->gs != NULL)
			continue;
		if(gs!=NULL&&strcmp(psi->gs,gs)!=0)
			continue;

		return psi->id;
	}

	ShaderInfo si;
	si.glId = shader_import(vs, fs, gs);
	si.id = currentShaderId++;

	si.referenceCount = 1;

	if (vs != NULL)
	{
		si.vs = malloc(sizeof(char) * (strlen(vs) + 1));
		strcpy(si.vs, vs);
	}
	else
		si.vs = NULL;

	if (fs != NULL)
	{
		si.fs = malloc(sizeof(char) * (strlen(fs) + 1));
		strcpy(si.fs, fs);
	}
	else
		si.fs = NULL;

	if (gs != NULL)
	{
		si.gs = malloc(sizeof(char) * (strlen(gs) + 1));
		strcpy(si.gs, gs);
	}
	else
		si.gs = NULL;


	seqtor_push_back(registeredShaders, si);
	return si.id;
}

void renderer_destroyShader(shader_id shader)
{
	int index = -1;
	for (int i = 0; i < seqtor_size(registeredShaders); i++)
	{
#define _s seqtor_at(registeredShaders, i)
		if (_s.id == shader)
		{
			if (0 == --_s.referenceCount)
			{
				free(_s.vs);
				free(_s.fs);
				if (_s.gs != NULL)
					free(_s.gs);

				shader_delete(_s.glId);
				
				seqtor_remove_at(registeredShaders, i);
			}
			
			break;
		}
#undef _s
	}
}

void renderer_useShader(shader_id shader)
{
	ShaderInfo* info = NULL;
	for (int i = 0; i < seqtor_size(registeredShaders); i++)
	{
		if (seqtor_at(registeredShaders, i).id == shader)
		{
			info = &(seqtor_at(registeredShaders, i));
			break;
		}
	}

	if (info == NULL)
	{
		fprintf(stderr, "Shader ID is not registered\n");
		return;
	}

	glUseProgram(info->glId);
	glUniform1i(glGetUniformLocation(info->glId, "tex"), 0);
	if (camera != NULL)
	{
		glUniformMatrix4fv(glGetUniformLocation(info->glId, "view"), 1, GL_FALSE, camera_getViewMatrix(camera).data);
		glUniformMatrix4fv(glGetUniformLocation(info->glId, "projection"), 1, GL_FALSE, camera_getProjectionMatrix(camera).data);
	}

	currentShader = info->glId;
}

void renderer_setRenderMode(GLenum renderMode)
{
	drawMode = renderMode;
}

void renderer_renderObject(struct Renderable renderable, struct Mat4 model)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderable.texture);

	glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, model.data);

	glBindVertexArray(renderable.vao);
	if (renderable.eboUsed == 0)
		glDrawArrays(drawMode, 0, renderable.count);
	else
		glDrawElements(drawMode, renderable.count, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void renderer_setCamera(struct Camera* cum)
{
	camera = cum;
}

Renderable renderer_createRenderable(const float* vData, unsigned int vCount, const unsigned int* iData, unsigned int iCount)
{
	Renderable r;

	glGenVertexArrays(1, &r.vao);
	glBindVertexArray(r.vao);
	
	glGenBuffers(1, &r.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vCount, vData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (iData == NULL)//no ebo is used
	{
		r.eboUsed = 0;
		r.ebo = 0;
		r.count = iCount;
	}
	else
	{
		glGenBuffers(1, &r.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * iCount, iData, GL_STATIC_DRAW);

		r.eboUsed = 1;
		r.count = iCount;
	}

	glBindVertexArray(0);
	return r;
}

void renderer_destroyRenderable(Renderable renderable)
{
	glDeleteVertexArrays(1, &renderable.vao);
	glDeleteBuffers(1, &renderable.vbo);
	if (renderable.eboUsed)
		glDeleteBuffers(1, &renderable.ebo);
}


texture_t renderer_createTexture(const char* imagePath, int channels)
{
	GLenum format;
	switch (channels)
	{
	case 3:
		format = GL_RGB;
		break;
	default:
		format = GL_RGBA;
		break;
	}

	return textureHandler_loadImage(imagePath, GL_RGBA, GL_RGBA, GL_LINEAR, 69);
}

void renderer_destroyTexture(texture_t texture)
{
	textureHandler_deleteImage(texture);
}

void renderer_setTexture(Renderable* renderable, texture_t texture)
{
	renderable->texture = texture;
}