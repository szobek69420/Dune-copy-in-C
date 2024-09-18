#include "player.h"

#include <stdlib.h>


#include "../game_object.h"
#include "../../Physics/physics.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"
#include "../../Camera/camera.h"

#include "../../Glm2/mat4.h"

struct Player {
	Transform transform;
	Renderable renderable;
	Collider* collider;
};
typedef struct Player Player;


const float vertices[] = {
	0,0,0,0.5f,0.5f,
	1.000f, 0.000f, 0.000f, 1.000f, 0.500f,
	0.951f, -0.309f, 0.000f, 0.976f, 0.345f,
	0.809f, -0.588f, 0.000f, 0.905f, 0.206f,
	0.588f, -0.809f, 0.000f, 0.794f, 0.095f,
	0.309f, -0.951f, 0.000f, 0.655f, 0.024f,
	-0.000f, -1.000f, 0.000f, 0.500f, 0.000f,
	-0.309f, -0.951f, 0.000f, 0.345f, 0.024f,
	-0.588f, -0.809f, 0.000f, 0.206f, 0.095f,
	-0.809f, -0.588f, 0.000f, 0.095f, 0.206f,
	-0.951f, -0.309f, 0.000f, 0.024f, 0.345f,
	-1.000f, 0.000f, 0.000f, 0.000f, 0.500f,
	-0.951f, 0.309f, 0.000f, 0.024f, 0.655f,
	-0.809f, 0.588f, 0.000f, 0.095f, 0.794f,
	-0.588f, 0.809f, 0.000f, 0.206f, 0.905f,
	-0.309f, 0.951f, 0.000f, 0.345f, 0.976f,
	0.000f, 1.000f, 0.000f, 0.500f, 1.000f,
	0.309f, 0.951f, 0.000f, 0.655f, 0.976f,
	0.588f, 0.809f, 0.000f, 0.794f, 0.905f,
	0.809f, 0.588f, 0.000f, 0.905f, 0.794f,
	0.951f, 0.309f, 0.000f, 0.976f, 0.655f,
	1.000f, 0.000f, 0.000f, 1.000f, 0.500f
};

const float vertices2[] = {
	0,0,0,0.5f,0.5f,
	1,-1,0,1,0,
	-1,-1,0,0,0,
	-1,1,0,0,1,
	1,1,0,1,1
};

extern void* MAIN_CUM;

//prototypes
void checkForScreenResize();

void* player_create()
{
	Player* player = (Player*)malloc(sizeof(Player));

	player->renderable = renderer_createRenderable(vertices, 110, NULL, 22);
	player->renderable.texture = renderer_createTexture("Assets/Sprites/player.png", 4);


	Vec3 helper;
	player->collider = physics_createBallCollider();
	helper = (Vec3){ 3,40,0 };
	physics_setColliderParam(player->collider, POSITION_VEC3, &helper);
	helper = (Vec3){ 0,0.0f,0 };
	physics_setColliderParam(player->collider, VELOCITY_VEC3, &helper);
	helper.x = 1;
	physics_setColliderParam(player->collider, RADIUS_FLOAT, &helper.x);
	helper.x = 0;
	physics_setColliderParam(player->collider, BOUNCINESS_FLOAT, &helper.x);
	
	return player;
}

void player_destroy(void* _player)//releases resources
{
	Player* player = _player;
	renderer_destroyRenderable(player->renderable);
	physics_destroyCollider(player->collider);
	free(player);
}


void player_update(void* _player, float deltaTime)
{
	Player* player = (Player*)_player;
	physics_getColliderParam(player->collider, POSITION_VEC3, &player->transform.position);

	//gravity
	Vec3 velocity;
	physics_getColliderParam(player->collider, VELOCITY_VEC3, &velocity);
	velocity = vec3_sum(velocity, (Vec3) { 0, -9.80625f * deltaTime, 0 });
	physics_setColliderParam(player->collider, VELOCITY_VEC3, &velocity);

	checkForScreenResize();
}

void player_onStart(void* _player)
{
	Player* player = (Player*)_player;
	player->transform.position = (Vec3){ 0,0,0 };
	player->transform.rotation = (Quat){ 1,0,0,0 };

	physics_setBouncinessCombine(BC_MULT);

	camera_setPosition(MAIN_CUM, (Vec3) { 15, 15, 0 });
}

void player_onDestroy(void* player)//do something ingame (the destroy() releases the resources)
{

}

void checkForScreenResize()
{
	static int previousWidth = 0, previousHeight = 0;

	if (window_width() == previousWidth && window_height() == previousHeight)
		return;

	previousWidth = window_width();
	previousHeight = window_height();

	float projectionHeight = 25;
	float projectionWidth = ((float)previousWidth / previousHeight) * projectionHeight;

	camera_setProjection(MAIN_CUM, projectionWidth, projectionHeight, 0, 10);
}



void player_render(void* player)
{
	Mat4 model = gameObject_getTransformWorldModel(&((Player*)player)->transform);

	renderer_useShader(0);
	renderer_setRenderMode(GL_TRIANGLE_FAN);

	renderer_renderObject(((Player*)player)->renderable, model);
}