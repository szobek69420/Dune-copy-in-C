#include "image.h"

#include <seqtor.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>


#include "../../Glm2/mat4.h"
#include "../ui.h"
#include "../../Renderer/Texture/texture_handler.h"
#include "../../Renderer/Shader/shader.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"

struct Image {
	UIComponent component;
	texture_t texture;
	float r, g, b, a;
};

typedef struct Image Image;


static const float vertices[] = {
	0,0,0,1,1,1,1,0
};


static unsigned int VAO = 0, VBO = 0;
static shader_id SHADER_ID = 0;
static shader_t SHADER = 0;
static texture_t TEXTURE_DEFAULT=0;
static Mat4 SCREEN_MATRIX;

static int INSTANCE_COUNT = 0;



void init();
void deinit();

void image_render(void* element, int minX, int minY, int maxX, int maxY);
void image_destroy(void* element);


void init()
{
	SCREEN_MATRIX = mat4_ortho(0, window_width(), window_height(), 0, -1, 1);

	SHADER_ID = renderer_createShader("Assets/Shaders/Image/image.vag", "Assets/Shaders/Image/image.fag", NULL);
	SHADER = renderer_getShaderGLId(SHADER_ID);
	renderer_useShader(SHADER_ID);
	glUniform1i(glGetUniformLocation(SHADER, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(SHADER, "projection"), 1, GL_FALSE, SCREEN_MATRIX.data);
	renderer_useShader(0);

	TEXTURE_DEFAULT = textureHandler_loadImage("Assets/System/UI/image_default.png", GL_RGBA, GL_RGBA, GL_LINEAR, 69);
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void deinit()
{
	renderer_destroyShader(SHADER_ID);
	SHADER = 0;

	textureHandler_deleteImage(TEXTURE_DEFAULT);
	TEXTURE_DEFAULT = 0;

	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	glDeleteBuffers(1, &VBO);
	VBO = 0;
}


void* image_create(const char* name)
{
	Image* image = malloc(sizeof(Image));
	if (image == NULL)
	{
		printf("Image: Image could not be created\n");
		return NULL;
	}

	image->component = ui_initComponent(name);
	image->texture = 0;
	image->r = 1;
	image->g = 1;
	image->b = 1;
	image->a = 1;

	image->component.render = image_render;
	image->component.destroy = image_destroy;


	if (INSTANCE_COUNT == 0)
		init();
	INSTANCE_COUNT++;

	return image;
}

void image_render(void* element, int minX, int minY, int maxX, int maxY)
{
	Image* image = element;

	int currentX = 0;
	int currentY = 0;

	//currentX and currentY have to be transformed so that they point to the top left of the thing
	switch (image->component.hAlign)
	{
	case ALIGN_LEFT:
		currentX = minX + image->component.xPos;
		break;

	case ALIGN_CENTER:
		currentX = (minX + maxX) / 2 + image->component.xPos - image->component.width / 2;
		break;

	case ALIGN_RIGHT:
		currentX = maxX - image->component.xPos - image->component.width;
		break;
	}

	switch (image->component.vAlign)
	{
	case ALIGN_TOP:
		currentY = minY + image->component.yPos;
		break;

	case ALIGN_CENTER:
		currentY = (minY + maxY) / 2 + image->component.yPos - image->component.height / 2;
		break;

	case ALIGN_BOTTOM:
		currentY = maxY - image->component.yPos - image->component.height;
		break;
	}

	glBindVertexArray(VAO);
	renderer_useShader(SHADER_ID);
	glUniform4f(glGetUniformLocation(SHADER, "colour"), image->r, image->g, image->b, image->a);
	glUniform4f(glGetUniformLocation(SHADER, "screenInfo"), currentX, currentY, image->component.width, image->component.height);
	glUniformMatrix4fv(glGetUniformLocation(SHADER, "projection"), 1, GL_FALSE, SCREEN_MATRIX.data);
	
	glActiveTexture(GL_TEXTURE0);
	if (image->texture == 0)
		glBindTexture(GL_TEXTURE_2D, TEXTURE_DEFAULT);
	else
		glBindTexture(GL_TEXTURE_2D, image->texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	renderer_useShader(0);
}

void image_destroy(void* element)
{
	Image* image = element;

	ui_destroyComponent(&image->component);
	free(image);

	INSTANCE_COUNT--;
	if (INSTANCE_COUNT == 0)
		deinit();
}


void image_setScreenSize(int width, int height)
{
	SCREEN_MATRIX = mat4_ortho(0, width, height, 0, -1, 1);
}
