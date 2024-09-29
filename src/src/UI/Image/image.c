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
	int shouldDestroyTexture;//if the texture has been imported here, it will also be released
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

	ui_calculateBounds(image, &minX, &minY, &maxX, &maxY);

	glBindVertexArray(VAO);
	renderer_useShader(SHADER_ID);
	glUniform4f(glGetUniformLocation(SHADER, "colour"), image->r, image->g, image->b, image->a);
	glUniform4f(glGetUniformLocation(SHADER, "screenInfo"), minX, minY, image->component.width, image->component.height);
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

	if (image->shouldDestroyTexture != 0)
		textureHandler_deleteImage(image->texture);

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

void image_setTexturePath(void* element, const char* texturePath, int channels)
{
	Image* image = element;
	if (image->shouldDestroyTexture !=0)
		textureHandler_deleteImage(image->texture);

	image->texture = textureHandler_loadImage(texturePath, channels == 4 ? GL_RGBA : GL_RGB, GL_RGBA, GL_LINEAR, 69);
	image->shouldDestroyTexture = 69;
}
void image_setTextureId(void* element, unsigned int textureId)
{
	Image* image = element;
	if (image->shouldDestroyTexture != 0)
		textureHandler_deleteImage(image->texture);

	image->texture = textureId;
	image->shouldDestroyTexture = 0;
}


void image_setColour(void* element, float r, float g, float b, float a)
{
	Image* image = element;
	image->r = r;
	image->g = g;
	image->b = b;
	image->a = a;
}

void image_getColour(void* element, float* r, float* g, float* b, float* a)
{
	Image* image = element;
	*r = image->r;
	*g = image->g;
	*b = image->b;
	*a = image->a;
}
