#include "player.h"

#include <stdlib.h>


#include "../game_object.h"
#include "../../Physics/physics.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"
#include "../../Camera/camera.h"
#include "../../Input/input.h"

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
static float aspectRatio = 1;
static float currentCameraHeight = 30;

static float DELTA_TIME = 0.0f;

//prototypes
void checkForScreenResize();
void updateCameraProperties(Player* player);
void handleInput(Player* player);
void applyGravityAndDrag(Player* player);

void* player_create()
{
	Player* player = (Player*)malloc(sizeof(Player));

	player->renderable = renderer_createRenderable(vertices, 110, NULL, 22);
	player->renderable.texture = renderer_createTexture("Assets/Sprites/player.png", 4);


	player->collider = physics_createBallCollider();
	
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
	DELTA_TIME = deltaTime;

	Player* player = (Player*)_player;
	physics_getColliderParam(player->collider, POSITION_VEC3, &player->transform.position);//update renderable position

	applyGravityAndDrag(player);
	handleInput(player);

	checkForScreenResize();
	updateCameraProperties(player);
}

void player_onStart(void* _player)
{
	Player* player = (Player*)_player;
	player->transform.position = (Vec3){ 0,0,0 };
	player->transform.rotation = (Quat){ 1,0,0,0 };

	physics_setBouncinessCombine(BC_MIN);

	Vec3 helper;
	helper = (Vec3){ 5,50,0 };
	physics_setColliderParam(player->collider, POSITION_VEC3, &helper);
	helper = (Vec3){ 0,0.0f,0 };
	physics_setColliderParam(player->collider, VELOCITY_VEC3, &helper);
	helper.x = 1;
	physics_setColliderParam(player->collider, RADIUS_FLOAT, &helper.x);
	helper.x = 0.1f;
	physics_setColliderParam(player->collider, BOUNCINESS_FLOAT, &helper.x);
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

	aspectRatio = (float)previousWidth / previousHeight;
}

void updateCameraProperties(Player* player)
{
	float height = player->transform.position.y + 10.0f;
	float width = height * aspectRatio;

	camera_setProjection(MAIN_CUM, width, height, 0, 10);
	
	Vec3 pos = (Vec3){ player->transform.position.x - 5.0f + 0.5f * width,height * 0.5f,0 };
	camera_setPosition(MAIN_CUM, pos);
}

void handleInput(Player* player)
{
	if (input_isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) != 0)
	{
		int collisionCount;
		CollisionInfo* collisions = physics_getColliderCollisions(player->collider, &collisionCount);

		Vec3 velocity;
		physics_getColliderParam(player->collider, VELOCITY_VEC3, &velocity);
		if (collisionCount == 0)
			velocity = vec3_sum(velocity, vec3_scale((Vec3) { 0, -1, 0 }, 30.0f * DELTA_TIME));
		else
			velocity = vec3_sum(velocity, vec3_scale((Vec3) { 1, 0, 0 }, 30.0f * DELTA_TIME));
		physics_setColliderParam(player->collider, VELOCITY_VEC3, &velocity);

		free(collisions);
	}
}

void applyGravityAndDrag(Player* player)
{
	Vec3 velocity;
	physics_getColliderParam(player->collider, VELOCITY_VEC3, &velocity);

	//gravity
	velocity = vec3_sum(velocity, (Vec3) { 0, -9.80625f * DELTA_TIME, 0 });

	//drag
	Vec3 drag = vec3_scale(velocity, -0.001f * vec3_sqrMagnitude(velocity) * DELTA_TIME);
	velocity = vec3_sum(velocity, drag);

	physics_setColliderParam(player->collider, VELOCITY_VEC3, &velocity);
}



void player_render(void* player)
{
	Mat4 model = gameObject_getTransformWorldModel(&((Player*)player)->transform);

	renderer_useShader(0);
	renderer_setRenderMode(GL_TRIANGLE_FAN);

	renderer_renderObject(((Player*)player)->renderable, model);
}