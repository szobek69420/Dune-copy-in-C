#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <seqtor.h>
#include "Shader/shader.h"

struct ShaderInfo {
	shader_id id;//used by the outside world
	shader_t glId;//given by opengl
};
typedef struct ShaderInfo ShaderInfo;

static seqtor_of(ShaderInfo) registeredShaders;
static unsigned int currentShaderId = 0;

static int isEboUsed = 1;
static GLenum drawMode = GL_TRIANGLES;

static Camera* camera=NULL;


void renderer_init()
{
	seqtor_init(registeredShaders, 1);
	renderer_createShader("Shaders/default.vag", "Shaders/default.fag", NULL);
}

void renderer_deinit()
{
	for (int i = 0; i < seqtor_size(registeredShaders); i++)
		shader_delete(seqtor_at(registeredShaders, i).glId);
	seqtor_destroy(registeredShaders);
}

shader_id renderer_createShader(const char* vs, const char* fs, const char* gs)//gives back the id with which the shader can be accessed
{
	ShaderInfo si;
	si.glId = shader_import(vs, fs, gs);
	si.id = currentShaderId++;
	seqtor_push_back(registeredShaders, si);
	return si.id;
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
}

void renderer_setRenderConfig(int eboUsed, GLenum renderMode)
{
	isEboUsed = eboUsed;
	drawMode = renderMode;
}

void renderer_renderObject(struct Renderable renderable, struct Mat4 model)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ide vmi);

	glBindVertexArray(renderable.vao);
	if (isEboUsed == 0)
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