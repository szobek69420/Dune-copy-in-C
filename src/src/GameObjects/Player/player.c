#define _CRT_SECURE_NO_WARNINGS

#include "player.h"

#include <stdlib.h>
#include <math.h>


#include "../game_object.h"
#include "../../Physics/physics.h"
#include "../../Renderer/renderer.h"
#include "../../Renderer/Window/window.h"
#include "../../Camera/camera.h"
#include "../../Input/input.h"

#include "../../Glm2/mat4.h"

#include "../../Renderer/Fonts/fonts.h"
#include "../../GameLoop/game_loop.h"

#include "../../UI/ui.h"
#include "../../UI/Text/text.h"

#define RAD2DEG 57.2957795f

#define TRAIL_LENGTH 20
#define TRAIL_VERTEX_COUNT 2*TRAIL_LENGTH
#define TRAIL_VERTEX_FLOAT_COUNT 5

struct Player {
	Transform transform;
	Renderable renderable;
	Collider* collider;

	int touchingGrass;
	float angularVelocity;

	Renderable trail;
	Vec3 trailPoints[TRAIL_LENGTH];

	void* playerInfoText;
};
typedef struct Player Player;

static int instanceCount = 0;
static shader_id trailShader;


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
void rotatePlayer(Player* player);
void updateTrail(void* _player);

void* player_create()
{
	Player* player = (Player*)malloc(sizeof(Player));

	player->renderable = renderer_createRenderable(vertices, 110, NULL, 22,0);
	player->renderable.texture = renderer_createTexture("Assets/Sprites/player.png", 4);


	player->collider = physics_createBallCollider();


	player->touchingGrass = 0;
	player->angularVelocity = 0;

	player->playerInfoText = NULL;

	memset(player->trailPoints, 0, sizeof(player->trailPoints));
	void* temp = malloc(sizeof(float) * TRAIL_VERTEX_FLOAT_COUNT * TRAIL_VERTEX_COUNT);
	player->trail = renderer_createRenderable(temp, TRAIL_VERTEX_FLOAT_COUNT * TRAIL_VERTEX_COUNT, NULL, TRAIL_VERTEX_COUNT,1);
	player->trail.texture = renderer_createTexture("Assets/Sprites/player.png", 4);
	free(temp);

	if (instanceCount == 0)
		trailShader = renderer_createShader("Assets/Shaders/default.vag", "Assets/Shaders/trail.fag", NULL);
	instanceCount++;

	return player;
}

void player_destroy(void* _player)//releases resources
{
	Player* player = _player;
	renderer_destroyRenderable(player->renderable);
	physics_destroyCollider(player->collider);
	renderer_destroyRenderable(player->trail);
	free(player);
	
	instanceCount--;
	if(instanceCount==0)
		renderer_destroyShader(trailShader);
}


void player_update(void* _player, float deltaTime)
{
	DELTA_TIME = deltaTime;

	Player* player = (Player*)_player;
	physics_getColliderParam(player->collider, POSITION_VEC3, &player->transform.position);//update renderable position

	//check for grass touching
	int colliderCount;
	CollisionInfo* ci = physics_getColliderCollisions(player->collider, &colliderCount);
	if (colliderCount > 0)
	{
		player->touchingGrass += 2;
		if (player->touchingGrass > 6)
			player->touchingGrass = 6;
	}
	else
	{
		player->touchingGrass--;
		if (player->touchingGrass < 0)
			player->touchingGrass = 0;
	}
	free(ci);

	applyGravityAndDrag(player);
	handleInput(player);

	rotatePlayer(player);

	updateTrail(player);

	checkForScreenResize();
	updateCameraProperties(player);


	//update playerinfo
	if (player->playerInfoText != NULL)
	{
		char buffer[50];
		sprintf_s(buffer, 50, "Position: %.1f, %.1f", player->transform.position.x, player->transform.position.y);
		text_setText(player->playerInfoText, buffer);
	}


	//yeet game
	if (input_isKeyPressed(GLFW_KEY_C))
		gameLoop_setCurrentStage(GS_DEINIT);
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

	helper = (Vec3){ 5,50,0 };
	for (int i = 0; i < TRAIL_LENGTH; i++)
		player->trailPoints[i] = helper;

	player->playerInfoText = ui_getElementByName("playerInfo");
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
	if (width < 60)//hogy a labda max a kepernyo harmadaig menjen elore
	{
		height *= 60.0f / width;
		width = 60.0f;
	}

	camera_setProjection(MAIN_CUM, width, height, 0, 10);
	
	Vec3 pos = (Vec3){ player->transform.position.x - 20.0f + 0.5f * width,height * 0.5f,0 };
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
		if (player->touchingGrass == 0)
			velocity = vec3_sum(velocity, vec3_scale((Vec3) { 0, -1, 0 }, 50.0f * DELTA_TIME));
		else
		{
			if(collisionCount==0||vec3_magnitude(collisions[0].collisionForce)<0.0001f)
				velocity = vec3_sum(velocity, vec3_scale((Vec3) { 1, 0, 0 }, 100.0f * DELTA_TIME));
			else
			{
				Vec3 temp = vec3_normalize(collisions[0].collisionForce);
				temp = vec3_scale((Vec3){ temp.y, -temp.x, temp.z }, 100.0f * DELTA_TIME);
				
				velocity = vec3_sum(velocity, temp);

				//printf("force applied: %.3f %.3f %.3f\n", temp.x / DELTA_TIME, temp.y / DELTA_TIME, temp.z / DELTA_TIME);
			}
		}
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
	Vec3 drag = vec3_scale(vec3_normalize(velocity), -0.02f * vec3_magnitude(velocity) * DELTA_TIME);
	velocity = vec3_sum(velocity, drag);

	physics_setColliderParam(player->collider, VELOCITY_VEC3, &velocity);
}

void rotatePlayer(Player* player)
{
	if (player->touchingGrass != 0)
	{
		int collisionCount;
		CollisionInfo* infos = physics_getColliderCollisions(player->collider, &collisionCount);
		if (collisionCount > 0 && vec3_magnitude(infos[0].collisionForce) > 0.0001f)
		{
			Vec3 temp = vec3_normalize(infos[0].collisionForce);
			temp = (Vec3){ temp.y,-temp.x,0 };
			
			Vec3 velocity;
			physics_getColliderParam(player->collider, VELOCITY_VEC3, &velocity);

			float newAngularVelocity = -vec3_dot(temp, velocity);//nem kell osztani a sugarral, mert 1

			player->angularVelocity += 0.1f * (newAngularVelocity - player->angularVelocity);
		}
		free(infos);
	}
	else
	{
		if (player->angularVelocity > 0)
			player->angularVelocity -= DELTA_TIME*5;
		else
			player->angularVelocity += DELTA_TIME*5;
		
		if (fabsf(player->angularVelocity) < 5*DELTA_TIME + 0.0001f)
			player->angularVelocity = 0;
	}
	
	player->transform.rotation = quat_multiply(quat_initRotation(player->angularVelocity * DELTA_TIME, (Vec3) { 0, 0, 1 }), player->transform.rotation);
}

void updateTrail(void* _player)
{
	Player* player = _player;

	Vec3 velocity;
	Vec3 position;
	physics_getColliderParam(player->collider, VELOCITY_VEC3, &velocity);
	physics_getColliderParam(player->collider, POSITION_VEC3, &position);

	float minDistance = 0.02f * vec3_magnitude(velocity);
	if (minDistance > vec3_magnitude(vec3_subtract(position, player->trailPoints[TRAIL_LENGTH - 1])))
		return;

	for (int i = 0; i < TRAIL_LENGTH - 1; i++)//push the points left one slot
		player->trailPoints[i] = player->trailPoints[i + 1];
	player->trailPoints[TRAIL_LENGTH - 1] = position;


	float trailLength = 0;
	for (int i = 0; i < TRAIL_LENGTH - 1; i++)
		trailLength += vec3_magnitude(vec3_subtract(player->trailPoints[i], player->trailPoints[i + 1]));

	float* vData = malloc(sizeof(float) * TRAIL_VERTEX_FLOAT_COUNT * TRAIL_VERTEX_COUNT);
	float currentLength = 0;
	Vec3 trailNormal;
	for (int i = 0, j=0; i < TRAIL_LENGTH; i++)
	{
		if(i!=0)
			currentLength+= vec3_magnitude(vec3_subtract(player->trailPoints[i], player->trailPoints[i - 1]));
		if (i < TRAIL_LENGTH - 1)
		{
			trailNormal = vec3_subtract(player->trailPoints[i + 1], player->trailPoints[i]);
			if (vec3_sqrMagnitude(trailNormal) > 0.0001f)
			{
				trailNormal = vec3_normalize(trailNormal);
				trailNormal = (Vec3){ -trailNormal.y,trailNormal.x,0 };
			}
			else
				trailNormal = (Vec3){ 0,1,0 };
		}

		float uvX = currentLength / trailLength;
		float transparency = uvX;

		Vec3 delta = vec3_scale(trailNormal, 0.4f * uvX + 0.5f);
		*(Vec3*)&vData[j] = vec3_sum(player->trailPoints[i],delta);
		vData[j + 3] = uvX;
		vData[j + 4] = 1;
		j += TRAIL_VERTEX_FLOAT_COUNT;

		*(Vec3*)&vData[j] = vec3_subtract(player->trailPoints[i], delta);
		vData[j + 3] = uvX;
		vData[j + 4] = 0;
		j += TRAIL_VERTEX_FLOAT_COUNT;
	}

	renderer_updateGeometry(&player->trail, vData, TRAIL_VERTEX_FLOAT_COUNT * TRAIL_VERTEX_COUNT);

	free(vData);
}



void player_render(void* _player)
{
	Player* player = _player;
	
	Mat4 parentModel = gameObject_getTransformWorldModel(player->transform.parent);
	Mat4 model = mat4_multiply(parentModel, gameObject_getTransformModel(&player->transform));


	//trail
	float trailLength = 0;
	for (int i = 0; i < TRAIL_LENGTH - 1; i++)
		trailLength += vec3_magnitude(vec3_subtract(player->trailPoints[i], player->trailPoints[i + 1]));
	if (trailLength > 0.1f)
	{
		renderer_useShader(trailShader);
		renderer_setRenderMode(GL_TRIANGLE_STRIP);
		renderer_setBlending(69);
		renderer_renderObject(player->trail, parentModel);
		renderer_setBlending(0);
	}

	//player
	renderer_useShader(0);
	renderer_setRenderMode(GL_TRIANGLE_FAN);

	renderer_renderObject(player->renderable, model);
}