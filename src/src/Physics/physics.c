#include "physics.h"

#include "../Glm2/vec3.h"
#include <seqtor.h>

struct BoundingBox {
	Vec3 lowerBound, upperBound;
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

}

Collider* physics_createPolygonCollider(const Vec3* points, int pointCount)
{

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

void physics_getBallParam(BallColliderParameter paramType, void* pbuffer)
{

}

void physics_setBallParam(BallColliderParameter paramType, void* pvalue)
{

}

void physics_getPolygonParam(BallColliderParameter paramType, void* pbuffer)
{

}

void physics_setPolygonParam(BallColliderParameter paramType, void* pvalue)
{

}