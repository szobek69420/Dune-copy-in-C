#include "background.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../game_object.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"

#include "../../Camera/camera.h"

#include "../../Glm2/vec3.h"
#include "../../Glm2/mat4.h"

struct Background {
	Transform transform;
	GameObjectFunctions functions;

	Renderable layer1;
	float layer1_offset;
	float layer1_scale;

	Renderable layer2;
	float layer2_offset;
	float layer2_scale;

	Renderable sun;
	Mat4 sunModel;
};
typedef struct Background Background;


void background_destroy(void* background);
void background_update(void* background, float deltaTime);
void background_render(void* background);


#define HEIGHT_LAYER_1 50.0f
#define SCROLL_LAYER_1 0.003f
static const float vertices_layer1[20];

#define HEIGHT_LAYER_2 100.0f
#define SCROLL_LAYER_2 0.001f
static const float vertices_layer2[20];

#define SUN_SCALE 30.0f
#define SUN_ALTITUDE 120.0f
static const float vertices_sun[20];


extern void* MAIN_CUM;

static int INSTANCE_COUNT = 0;

static texture_t TEXTURE_LAYER_1 = 0;
static texture_t TEXTURE_LAYER_2 = 0;
static texture_t TEXTURE_SUN = 0;

static shader_id SHADER_ID = 0;
static shader_t SHADER_GLID = 0;

static shader_id SUN_SHADER_ID = 0;


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
		TEXTURE_LAYER_2 = renderer_createTexture("Assets/Sprites/Background/layer2.png", 69);
		TEXTURE_SUN = renderer_createTexture("Assets/Sprites/Background/sun.png", 69);

		SHADER_ID = renderer_createShader("Assets/Shaders/Background/background.vag", "Assets/Shaders/Background/background.fag", NULL);
		SHADER_GLID = renderer_getShaderGLId(SHADER_ID);

		SUN_SHADER_ID= renderer_createShader("Assets/Shaders/default.vag", "Assets/Shaders/Background/sun.fag", NULL);
	}

	bg->layer1 = renderer_createRenderable(vertices_layer1, 20, NULL, 4, 0);
	bg->layer1.texture = TEXTURE_LAYER_1;
	bg->layer1_offset = 0;
	bg->layer1_scale = 1.0f;

	bg->layer2 = renderer_createRenderable(vertices_layer2, 20, NULL, 4, 0);
	bg->layer2.texture = TEXTURE_LAYER_2;
	bg->layer2_offset = 0;
	bg->layer2_scale = 1.0f;

	bg->sun= renderer_createRenderable(vertices_sun, 20, NULL, 4, 0);
	bg->sun.texture = TEXTURE_SUN;
	bg->sunModel = mat4_create(0);

	return bg;
}

void background_destroy(void* background)
{
	Background* bg = background;
	gameObject_destroyTransform(&bg->transform);

	renderer_destroyRenderable(bg->layer1);
	renderer_destroyRenderable(bg->layer2);
	
	free(bg);

	if (--INSTANCE_COUNT == 0)
	{
		renderer_destroyTexture(TEXTURE_LAYER_1);
		renderer_destroyTexture(TEXTURE_LAYER_2);
		renderer_destroyTexture(TEXTURE_SUN);
		renderer_destroyShader(SHADER_ID);
		renderer_destroyShader(SUN_SHADER_ID);

		TEXTURE_LAYER_1 = 0;
		TEXTURE_LAYER_2 = 0;
		TEXTURE_SUN = 0;
		SHADER_ID = 0;
		SHADER_GLID = 0;
		SUN_SHADER_ID = 0;
	}
}

void background_update(void* background, float deltaTime)
{
	Background* bg = background;

	bg->layer1_scale = 0.75f*camera_getWidth(MAIN_CUM)/HEIGHT_LAYER_1;
	bg->layer1_offset = SCROLL_LAYER_1 * (camera_getPosition(MAIN_CUM).x-0.5f*camera_getWidth(MAIN_CUM));

	bg->layer2_scale = camera_getWidth(MAIN_CUM) / HEIGHT_LAYER_2;
	bg->layer2_offset = SCROLL_LAYER_2 * (camera_getPosition(MAIN_CUM).x - 0.5f * camera_getWidth(MAIN_CUM));

	Vec3 sunPos;
	sunPos.x = camera_getPosition(MAIN_CUM).x + 0.5f * camera_getWidth(MAIN_CUM) - 30 - 0.5f * SUN_SCALE;
	if (sunPos.x - camera_getPosition(MAIN_CUM).x < 0.25f * camera_getWidth(MAIN_CUM))
		sunPos.x = camera_getPosition(MAIN_CUM).x + 0.25f * camera_getWidth(MAIN_CUM);
	sunPos.y = SUN_ALTITUDE;
	sunPos.z = 0;
	bg->sunModel = mat4_translate(mat4_create(1), sunPos);
}

void background_render(void* background)
{
	Background* bg = background;

	renderer_setRenderMode(GL_TRIANGLE_FAN);


	renderer_setBlending(69);
	renderer_useShader(SUN_SHADER_ID);
	renderer_renderObject(bg->sun, bg->sunModel);
	renderer_setBlending(0);


	renderer_useShader(SHADER_ID);

	glUniform2f(glGetUniformLocation(SHADER_GLID, "uv_info"), bg->layer2_offset, bg->layer2_scale);
	renderer_renderObject(bg->layer2, mat4_create(1));

	glUniform2f(glGetUniformLocation(SHADER_GLID, "uv_info"), bg->layer1_offset, bg->layer1_scale);
	renderer_renderObject(bg->layer1, mat4_create(1));

	renderer_useShader(0);
}

const float vertices_layer1[] = {
	-1.0f,HEIGHT_LAYER_1-1.0f,0,0.0f,1.0f,
	1.0f,HEIGHT_LAYER_1 - 1.0f,0,1.0f,1.0f,
	1.0f,-1.0f,0,1.0f,0.0f,
	-1.0f,-1.0f,0,0.0f,0.0f
};

const float vertices_layer2[] = {
	-1.0f,HEIGHT_LAYER_2 - 1.0f,0,0.0f,1.0f,
	1.0f,HEIGHT_LAYER_2 - 1.0f,0,1.0f,1.0f,
	1.0f,-1.0f,0,1.0f,0.0f,
	-1.0f,-1.0f,0,0.0f,0.0f
};

const float vertices_sun[] = {
	-0.5f*SUN_SCALE,0.5f * SUN_SCALE,0,0.0f,1.0f,
	0.5f * SUN_SCALE,0.5f * SUN_SCALE,0,1.0f,1.0f,
	0.5f * SUN_SCALE,-0.5f * SUN_SCALE,0,1.0f,0.0f,
	-0.5f * SUN_SCALE,-0.5f * SUN_SCALE,0,0.0f,0.0f
};

