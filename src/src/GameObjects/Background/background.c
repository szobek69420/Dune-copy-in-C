#include "background.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../game_object.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"

#include "../../Camera/camera.h"

struct Background {
	Transform transform;
	GameObjectFunctions functions;

	Renderable layer1;
	float layer1_offset;
	float layer1_scale;
};
typedef struct Background Background;


void background_destroy(void* background);
void background_update(void* background, float deltaTime);
void background_render(void* background);


static const float HEIGHT_LAYER_1 = 1.2f;
static const float SCROLL_LAYER_1 = 0.001f;
static const float vertices_layer1[20];

extern void* MAIN_CUM;

static int INSTANCE_COUNT = 0;

static texture_t TEXTURE_LAYER_1 = 0;

static shader_id SHADER_ID = 0;
static shader_t SHADER_GLID = 0;


void* background_create(const char* name)
{
	Background* bg = malloc(sizeof(Background));
	if(bg==NULL)
	{
		printf("Background: Background could not be created\n");
		return NULL;
	}

	bg->transform = gameObject_createTransform(name);
	
	memset(&bg->functions, 0, sizeof(GameObjectFunctions));
	bg->functions.destroy = background_destroy;
	bg->functions.render = background_render;
	bg->functions.onUpdate = background_update;


	if (INSTANCE_COUNT++ == 0)
	{
		TEXTURE_LAYER_1 = renderer_createTexture("Assets/Sprites/Background/layer1.png", 69);

		SHADER_ID = renderer_createShader("Assets/Shaders/Background/background.vag", "Assets/Shaders/Background/background.fag", NULL);
		SHADER_GLID = renderer_getShaderGLId(SHADER_ID);
	}

	bg->layer1 = renderer_createRenderable(vertices_layer1, 20, NULL, 4, 0);
	bg->layer1.texture = TEXTURE_LAYER_1;
	bg->layer1_offset = 0;
	bg->layer1_scale = 1.0f;

	return bg;
}

void background_destroy(void* background)
{
	Background* bg = background;
	gameObject_destroyTransform(&bg->transform);

	renderer_destroyRenderable(bg->layer1);
	
	free(bg);

	if (--INSTANCE_COUNT == 0)
	{
		renderer_destroyTexture(TEXTURE_LAYER_1);
		renderer_destroyShader(SHADER_ID);

		TEXTURE_LAYER_1 = 0;
		SHADER_ID = 0;
		SHADER_GLID = 0;
	}
}

void background_update(void* background, float deltaTime)
{
	Background* bg = background;

	bg->layer1_scale = 0.5f * HEIGHT_LAYER_1 * ((float)window_width() / window_height());
	bg->layer1_offset = SCROLL_LAYER_1 * camera_getPosition(MAIN_CUM).x;
}

void background_render(void* background)
{
	Background* bg = background;

	renderer_useShader(SHADER_ID);
	glUniform2f(glGetUniformLocation(SHADER_GLID, "uv_info"), bg->layer1_offset, bg->layer1_scale);
	renderer_setRenderMode(GL_TRIANGLE_FAN);
	renderer_renderObject(bg->layer1, mat4_create(1));

	renderer_useShader(0);
}

const float vertices_layer1[] = {
	-1.0f,0.2f,0,0.0f,1.0f,
	1.0f,0.2f,0,1.0f,1.0f,
	1.0f,-1.0f,0,1.0f,0.0f,
	-1.0f,-1.0f,0,0.0f,0.0f
};

