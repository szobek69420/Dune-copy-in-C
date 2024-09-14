#include "physics.h"

#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "../Glm2/vec3.h"
#include "../Glm2/vec4.h"
#include <seqtor.h>

struct BoundingBox {
	Vec4 bounds;//xy: lower.xy, zw: upper.xy
};
typedef struct BoundingBox BoundingBox;

struct BallInfo {
	float radius;
};
typedef struct BallInfo BallInfo;

struct PolygonInfo {
	Vec3* points;
	int pointCount;
};
typedef struct PolygonInfo PolygonInfo;

struct Collider {
	int type;
	int id;
	int isMovable;

	Vec3 position;
	BoundingBox boundingBox;//local to the collider's space

	seqtor_of(CollisionInfo) collisions;


	union {
		BallInfo ball;
		PolygonInfo polygon;
	};
};

void physics_calculateBoundingBox(Collider* collider);


static seqtor_of(Collider*) REGISTERED_COLLIDERS;
static int CURRENT_ID = 69420;

void physics_init()
{
	seqtor_init(REGISTERED_COLLIDERS, 1);
}

void physics_step(float deltaTime)
{

}

void physics_deinit()
{
	for (int i = seqtor_size(REGISTERED_COLLIDERS) - 1; i >= 0; i--)
		physics_destroyCollider(seqtor_at(REGISTERED_COLLIDERS, i));
	seqtor_destroy(REGISTERED_COLLIDERS);
}

Collider* physics_createBallCollider()
{
	Collider* collider = (Collider*)malloc(sizeof(Collider));

	collider->type = BALL;
	collider->id = CURRENT_ID++;

	collider->position = vec3_create(0);
	collider->isMovable = 1;
	physics_calculateBoundingBox(collider);

	seqtor_init(collider->collisions, 1);
	
	collider->ball.radius = 1;


	seqtor_push_back(REGISTERED_COLLIDERS, collider);

	return collider;
}

Collider* physics_createPolygonCollider(const Vec3* points, int pointCount)
{
	Collider* collider = (Collider*)malloc(sizeof(Collider));
	
	collider->type = POLYGON;
	collider->id = CURRENT_ID++;

	collider->position = vec3_create(0);
	collider->isMovable = 1;
	physics_calculateBoundingBox(collider);

	seqtor_init(collider->collisions, 1);

	collider->polygon.points = (Vec3*)malloc(pointCount * sizeof(Vec3));
	collider->polygon.pointCount = pointCount;
	memcpy(collider->polygon.points, points, pointCount * sizeof(Vec3));

	physics_calculateBoundingBox(collider);

	seqtor_push_back(REGISTERED_COLLIDERS, collider);

	return collider;
}

void physics_destroyCollider(Collider* collider)
{
	for (int i = 0; i < seqtor_size(REGISTERED_COLLIDERS); i++)//remove from registry
	{
		if (seqtor_at(REGISTERED_COLLIDERS, i)->id == collider->id)
		{
			seqtor_remove_at(REGISTERED_COLLIDERS, i);
			break;
		}
	}

	seqtor_destroy(collider->collisions);

	switch (collider->type)
	{
	case BALL:
		break;

	case POLYGON:
		free(collider->polygon.points);
		break;
	}

	free(collider);
}

void physics_getColliderParam(Collider* collider, ColliderParameter paramType, void* pbuffer)
{
	switch (paramType)
	{
		case POSITION_VEC3:
			*(Vec3*)pbuffer = collider->position;
			break;

		case MOVABLE_INT:
			*(int*)pbuffer = collider->isMovable;
			break;

		case RADIUS_FLOAT:
			if (collider->type != BALL)
			{
				printf("Physics: Invalid parameter type\n");
				break;
			}
			*(float*)pbuffer = collider->ball.radius;
			break;
	}
}

void physics_setColliderParam(Collider* collider, ColliderParameter paramType, void* pvalue)
{
	switch (paramType)
	{
		case POSITION_VEC3:
			collider->position = *(Vec3*)pvalue;
			break;

		case MOVABLE_INT:
			collider->isMovable = *(int*)pvalue;
			break;

		case RADIUS_FLOAT:
			if (collider->type != BALL)
			{
				printf("Physics: Invalid parameter type\n");
				break;
			}
			collider->ball.radius = *(float*)pvalue;
			break;
	}
}


void physics_calculateBoundingBox(Collider* collider)
{
	switch (collider->type)
	{
	case BALL:
		collider->boundingBox.bounds = (Vec4){
			collider->position.x - collider->ball.radius,
			collider->position.y - collider->ball.radius,
			collider->position.x + collider->ball.radius,
			collider->position.y + collider->ball.radius
		};
		break;

	case POLYGON:
		collider->boundingBox.bounds = (Vec4){ FLT_MAX,FLT_MAX,FLT_MIN,FLT_MIN };
		for (int i = 0; i < collider->polygon.pointCount; i++)
		{
			Vec3* point = &(collider->polygon.points[i]);

			if (point->x < collider->boundingBox.bounds.x)
				collider->boundingBox.bounds.x=point->x;
			else if(point->x>collider->boundingBox.bounds.z)
				collider->boundingBox.bounds.z = point->x;

			if (point->y < collider->boundingBox.bounds.y)
				collider->boundingBox.bounds.y = point->y;
			else if (point->y > collider->boundingBox.bounds.w)
				collider->boundingBox.bounds.w = point->y;
		}
		break;
	}
}