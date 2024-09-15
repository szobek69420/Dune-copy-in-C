#ifndef PHYSICS_H
#define PHYSICS_H

#include "../Glm2/vec3.h"

struct Collider;
typedef struct Collider Collider;

enum ColliderType {
	BALL, POLYGON
};
typedef enum ColliderType ColliderType;

enum ColliderParameter {
	POSITION_VEC3,
	VELOCITY_VEC3,
	MOVABLE_INT,

	RADIUS_FLOAT
};
typedef enum ColliderParameter ColliderParameter;


struct CollisionInfo {
	Collider* otherCollider; //if null that means that no collision has happened in the last physics update
	Vec3 collisionForce;
};
typedef struct CollisionInfo CollisionInfo;

void physics_init();
void physics_step(float deltaTime);
void physics_deinit();

Collider* physics_createBallCollider();
//the points should be given in a clockwise order
Collider* physics_createPolygonCollider(const Vec3* points, int pointCount);
void physics_destroyCollider(Collider* collider);

void physics_getColliderParam(Collider* collider, ColliderParameter paramType, void* pbuffer);
//pvalue must be the address of the value that is given to the function (for an int: int*, for a float array: float*)
void physics_setColliderParam(Collider* collider, ColliderParameter paramType, void* pvalue);

//count is a buffer, in which the number of collisions gets written. THE RETURN VALUE HAS TO BE FREED MANUALLY!!!
CollisionInfo* physics_getColliderCollisions(Collider* collider, int* count);

#endif