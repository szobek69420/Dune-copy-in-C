#include "player.h"

#include <stdlib.h>

#include "../../Renderer/Window/window.h"
#include "../../Camera/camera.h"


const float vertices[] = {
	-1, -1, 0, 0, 0,
	1, -1, 0, 1, 0,
	1, 1, 0, 1, 1,
	-1, 1, 0, 0, 1
};
const unsigned int indices[] = {
	0, 2, 1, 2, 0, 3
};

extern void* MAIN_CUM;

//prototypes
void checkForScreenResize();

void* player_create()
{
	Player* player = (Player*)malloc(sizeof(Player));

	player->renderable = renderer_createRenderable(vertices, 20, indices, 6);
	player->renderable.texture = renderer_createTexture("Assets/Sprites/player.png", 4);

	return player;
}

void player_destroy(void* _player)
{
	Player* player = _player;
	gameObject_destroyTransform(&(player->transform));
	renderer_destroyRenderable(player->renderable);
	free(player);
}


void player_update(void* _player, float deltaTime)
{
	Player* player = (Player*)_player;
	player->transform.position.x += deltaTime * 2;

	checkForScreenResize();
}

void player_onStart(void* _player)
{
	Player* player = (Player*)_player;
	player->transform.position = (Vec3){ 0,0,0 };
	player->transform.rotation = (Quat){ 1,0,0,0 };
	player->transform.scale = (Vec3){ 1,1,1 };
}

void player_onDestroy(void* player)
{

}

void checkForScreenResize()
{
	static int previousWidth = 0, previousHeight = 0;

	if (window_width() == previousWidth && window_height() == previousHeight)
		return;

	previousWidth = window_width();
	previousHeight = window_height();

	float projectionHeight = 10;
	float projectionWidth = ((float)previousWidth / previousHeight) * projectionHeight;

	camera_setProjection(MAIN_CUM, projectionWidth, projectionHeight, 0, 10);
}



void player_render(void* player, Mat4 parentModel)
{
	renderer_useShader(0);
	renderer_setRenderMode(GL_TRIANGLES);
	renderer_renderObject(((Player*)player)->renderable, mat4_multiply(parentModel,gameObject_getTransformModel(&(((Player*)player)->transform))));
}