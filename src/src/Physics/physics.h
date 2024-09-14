#ifndef PHYSICS_H
#define PHYSICS_H

#include "../Glm2/vec3.h"

struct Collider;
typedef struct Collider Collider;

enum ColliderType {
	BALL, POLYGON
};
typedef enum ColliderType ColliderType;

enum BallColliderParameter {
	BALL_POSITION_VEC3,
	BALL_RADIUS_FLOAT,
	BALL_MOVABLE_INT
};
typedef enum BallColliderParameter BallColliderParameter;

enum PolygonColliderParameter {
	POLYGON_POSITION_VEC3,
	POLYGON_MOVABLE_INT
};
typedef enum PolygonColliderParameter PolygonColliderParameter;

struct CollisionInfo {
	Collider* otherCollider; //if null that means that no collision has happened in the last physics update
	Vec3 collisionForce;
};
typedef struct CollisionInfo CollisionInfo;

void physics_init();
void physics_step(float deltaTime);
void physics_deinit();

Collider* physics_createBallCollider();
Collider* physics_createPolygonCollider(const Vec3* points, int pointCount);
void physics_destroyCollider(Collider* collider);

void physics_getBallParam(BallColliderParameter paramType, void* pbuffer);
void physics_setBallParam(BallColliderParameter paramType, void* pvalue);

void physics_getPolygonParam(BallColliderParameter paramType, void* pbuffer);
void physics_setPolygonParam(BallColliderParameter paramType, void* pvalue);

#endif