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
	Vec3 velocity;
	BoundingBox boundingBox;//local to the collider's space

	seqtor_of(struct CollisionInfo) collisions;//only on movable colliders is this set


	union {
		BallInfo ball;
		PolygonInfo polygon;
	};
};


void physics_calculateBoundingBox(Collider* collider);
float physics_boundingBoxPenetration(const Collider* c1, const Collider* c2);
int physics_detectCollision(Collider* c1, Collider* c2);//returns -1 if no bounding box contact, 0 if no collision, 1 if collision

static Collider* COMPARED_COLLIDER=NULL;
int physics_stepHelper(const void* c1, const void* c2);//ez a fuggveny megnezi, hogy melyik collider van kozelebb a COMPARED_COLLIDER-hez a bounding boxok szerint


static seqtor_of(Collider*) REGISTERED_COLLIDERS;
static int CURRENT_ID = 69420;

static float DELTA_TIME = 0;

void physics_init()
{
	seqtor_init(REGISTERED_COLLIDERS, 1);
}

void physics_step(float deltaTime)
{
	const int LENGTH = seqtor_size(REGISTERED_COLLIDERS);
	DELTA_TIME = deltaTime;


	//clear previous collision info
	for (int i = 0; i < LENGTH; i++)
	{
		if (seqtor_size(seqtor_at(REGISTERED_COLLIDERS, i)->collisions) > 0)
		{
			seqtor_clear((seqtor_at(REGISTERED_COLLIDERS, i))->collisions);
			//printf("cleared\n");
		}
	}

	//add velocity to position
	for (int i = 0; i < LENGTH; i++)
		seqtor_at(REGISTERED_COLLIDERS, i)->position = vec3_sum(seqtor_at(REGISTERED_COLLIDERS, i)->position, vec3_scale(seqtor_at(REGISTERED_COLLIDERS, i)->velocity, DELTA_TIME));


	Collider** colliders = malloc(sizeof(Collider*) * LENGTH);
	memcpy(colliders, REGISTERED_COLLIDERS.data, LENGTH * sizeof(Collider*));

	for (int i = 0; i < LENGTH; i++)
	{
		Collider* current = seqtor_at(REGISTERED_COLLIDERS, i);
		COMPARED_COLLIDER = current;
		qsort(colliders, LENGTH, sizeof(Collider*), physics_stepHelper);

		for (int j = 0; j < LENGTH; j++)
		{
			Collider* otherCurrent = colliders[j];
			if (current == otherCurrent)
				continue;

			int collision = physics_detectCollision(current, otherCurrent);
			if (collision == -1)
				break;
		}
	}

	free(colliders);
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
	collider->velocity = vec3_create(0);
	collider->isMovable = 1;
	collider->ball.radius = 1;
	physics_calculateBoundingBox(collider);

	seqtor_init(collider->collisions, 1);
	


	seqtor_push_back(REGISTERED_COLLIDERS, collider);

	return collider;
}

Collider* physics_createPolygonCollider(const Vec3* points, int pointCount)
{
	Collider* collider = (Collider*)malloc(sizeof(Collider));
	
	collider->type = POLYGON;
	collider->id = CURRENT_ID++;

	collider->position = vec3_create(0);
	collider->velocity = vec3_create(0);
	collider->isMovable = 1;

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

		case VELOCITY_VEC3:
			*(Vec3*)pbuffer = collider->velocity;
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

		case VELOCITY_VEC3:
			collider->velocity = *(Vec3*)pvalue;
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

CollisionInfo* physics_getColliderCollisions(Collider* collider, int* count)
{
	*count = seqtor_size(collider->collisions);

	if (*count == 0)
		return NULL;

	CollisionInfo* collisions = malloc((*count) * sizeof(CollisionInfo));
	memcpy(collisions, collider->collisions.data, (*count) * sizeof(CollisionInfo));
	return collisions;
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


int physics_stepHelper(const void* c1, const void* c2)
{
	float szam = physics_boundingBoxPenetration(COMPARED_COLLIDER, c2) - physics_boundingBoxPenetration(COMPARED_COLLIDER, c1);
	
	if (szam > 0)
		return 1;
	if (szam == 0)
		return 0;
	return -1;
}


//collision detection
Vec3 physics_closestPointOfLineSegment(Vec3 point, Vec3 lineA, Vec3 lineB);

int physics_collisionBallBall(Collider* c1, Collider* c2);
int physics_collisionPolygonPolygon(Collider* c1, Collider* c2);
int physics_collisionBallPolygon(Collider* ball, Collider* polygon);

int physics_detectCollision(Collider* c1, Collider* c2)//returns -1 if no bounding box contact, 0 if no collision, 1 if collision
{
	if (physics_boundingBoxPenetration(c1, c2) <= 0)
		return -1;

	if (c1->isMovable == 0 && c2->isMovable == 0)
		return 0;

	int collision = 0;

	switch (c1->type)
	{
	case BALL:
		switch (c2->type)
		{
		case BALL:
			collision=physics_collisionBallBall(c1, c2);
			break;

		case POLYGON:
			collision = physics_collisionBallPolygon(c1, c2);
			break;
		}
		break;

	case POLYGON:
		switch (c2->type)
		{
		case BALL:
			collision = physics_collisionBallPolygon(c2, c1);
			break;

		case POLYGON:
			collision = physics_collisionPolygonPolygon(c1, c2);
			break;
		}
		break;
	}

	return collision;
}

float physics_boundingBoxPenetration(const Collider* c1, const Collider* c2)//negative value means no penetration
{
	Vec4 c1bb = c1->boundingBox.bounds;
	c1bb = (Vec4){ c1bb.x + c1->position.x,c1bb.y + c1->position.y,c1bb.z + c1->position.x, c1bb.w + c1->position.y };
	Vec4 c2bb = c2->boundingBox.bounds;
	c2bb = (Vec4){ c2bb.x + c2->position.x,c2bb.y + c2->position.y,c2bb.z + c2->position.x, c2bb.w + c2->position.y };

	float penetration = FLT_MAX;

	if (c1bb.x > c2bb.x && c1bb.x < c2bb.z)
	{
		if (penetration > c2bb.z - c1bb.x)
			penetration = c2bb.z - c1bb.x;
	}
	if (c1bb.z > c2bb.x && c1bb.z < c2bb.z)
	{
		if (penetration > c1bb.z - c2bb.x)
			penetration = c1bb.z - c2bb.x;
	}

	if (c1bb.y > c2bb.y && c1bb.y < c2bb.w)
	{
		if (penetration > c2bb.w - c1bb.y)
			penetration = c2bb.w - c1bb.y;
	}
	if (c1bb.y > c2bb.y && c1bb.w < c2bb.w)
	{
		if (penetration > c1bb.w - c2bb.y)
			penetration = c1bb.w - c2bb.y;
	}

	if (penetration == FLT_MAX)
		penetration = -69;

	return penetration;
}


int physics_collisionBallBall(Collider* c1, Collider* c2)//assumes that at least one of them is movable
{
	if (c2->isMovable == 0)
	{
		Collider* temp = c2;
		c2 = c1;
		c1 = temp;
	}

	Vec3 distanceVec = vec3_subtract(c2->position, c1->position);
	float distance = vec3_magnitude(distanceVec);

	if (distance >= c2->ball.radius + c1->ball.radius)
		return 0;

	Vec3 distanceNormal = vec3_normalize(distanceVec);
	Vec3 delta = vec3_scale(distanceNormal, (c2->ball.radius + c1->ball.radius) - distance);

	if (c1->isMovable == 0)
	{
		c2->position = vec3_sum(c2->position, delta);
		Vec3 prevVelocity = c2->velocity;
		c2->velocity = vec3_reflect(c2->velocity, distanceNormal);

		CollisionInfo ci;
		ci.otherCollider = c1;
		ci.collisionForce = vec3_scale(vec3_subtract(c2->velocity,prevVelocity), 1.0f / DELTA_TIME);

		seqtor_push_back(c2->collisions, ci);

		return 1;
	}

	delta = vec3_scale(delta, 0.5f);
	
	Vec3 c1Proj = vec3_scale(distanceNormal, vec3_dot(c1->velocity, distanceNormal));
	Vec3 c2Proj = vec3_scale(distanceNormal, vec3_dot(c2->velocity, distanceNormal));

	Vec3 c1PrevVelocity = c1->velocity;
	Vec3 c2PrevVelocity = c2->velocity;

	c1->velocity = vec3_subtract(c1->velocity, c1Proj);
	c1->velocity = vec3_sum(c1->velocity, c2Proj);

	c2->velocity = vec3_subtract(c2->velocity, c2Proj);
	c2->velocity = vec3_sum(c2->velocity, c1Proj);

	c1->position = vec3_sum(c1->position, (Vec3) { -1 * delta.x, -1 * delta.y, -1 * delta.z });
	c2->position = vec3_sum(c2->position, delta);


	CollisionInfo ci;

	ci.otherCollider = c2;
	ci.collisionForce = vec3_scale(vec3_subtract(c1->velocity, c1PrevVelocity), 1.0f / DELTA_TIME);
	seqtor_push_back(c1->collisions, ci);

	ci.otherCollider = c1;
	ci.collisionForce = vec3_scale(vec3_subtract(c2->velocity, c2PrevVelocity), 1.0f / DELTA_TIME);
	seqtor_push_back(c2->collisions, ci);

	return 1;
}

int physics_collisionPolygonPolygon(Collider* c1, Collider* c2)
{
	return 0;//idk ilyet nem tudok
}

int physics_collisionBallPolygon(Collider* ball, Collider* polygon)
{
	Vec3 resolutionDir = (Vec3){ 0,0,0 };
	float resolutionLength = FLT_MAX;

	Vec3 colliderPos = polygon->position;
	Vec3* points = malloc(sizeof(Vec3) * polygon->polygon.pointCount);
	memcpy(points, polygon->polygon.points, sizeof(Vec3) * polygon->polygon.pointCount);
	points[0] = (Vec3){ points[0].x + colliderPos.x,points[0].y + colliderPos.y,points[0].z + colliderPos.z };


	for (int i = 0; i < polygon->polygon.pointCount - 1; i++)
	{
		points[i+1] = (Vec3){ points[i+1].x + colliderPos.x,points[i+1].y + colliderPos.y,points[i+1].z + colliderPos.z };

		Vec3 delta = vec3_subtract(
			ball->position,
			physics_closestPointOfLineSegment(ball->position, points[i], points[i + 1])
		);

		float deltaLength = vec3_magnitude(delta);
		if (deltaLength >= ball->ball.radius)
			continue;

		Vec3 resolution;

		//a golyo kozeppontja a vonal belso oldalan van-e
		if (vec3_dot(
			delta,
			(Vec3) {-(points[i + 1].y - points[i].y), points[i + 1].x - points[i].x, 0}
		)<0)
		{
			resolution = vec3_scale(vec3_normalize(delta), -vec3_magnitude(delta) - ball->ball.radius);
		}
		else //ha nem
		{
			resolution = vec3_scale(vec3_normalize(delta), ball->ball.radius - deltaLength);
		}


		if (resolutionLength > vec3_magnitude(resolution))
		{
			resolutionDir = resolution;
			resolutionLength = vec3_magnitude(resolution);
		}
	}

	free(points);

	if (resolutionLength == FLT_MAX)
		return 0;

	//innentol kb a ballball-bol lett masolva
	Vec3 distanceNormal = vec3_normalize(resolutionDir);
	Vec3 delta = resolutionDir;

	Collider* c1 = polygon;
	Collider* c2 = ball;
	if (c2->isMovable == 0)
	{
		Collider* temp = c2;
		c2 = c1;
		c1 = temp;
		delta = (Vec3){ -1 * delta.x,-1 * delta.y,-1 * delta.z };
		distanceNormal = (Vec3){ -1 * distanceNormal.x,-1 * distanceNormal.y,-1 * distanceNormal.z };
	}

	if (c1->isMovable == 0)
	{
		c2->position = vec3_sum(c2->position, delta);
		Vec3 prevVelocity = c2->velocity;
		c2->velocity = vec3_reflect(c2->velocity, distanceNormal);

		CollisionInfo ci;
		ci.otherCollider = c1;
		ci.collisionForce = vec3_scale(vec3_subtract(c2->velocity, prevVelocity), 1.0f / DELTA_TIME);

		seqtor_push_back(c2->collisions, ci);

		return 1;
	}

	delta = vec3_scale(delta, 0.5f);

	Vec3 c1Proj = vec3_scale(distanceNormal, vec3_dot(c1->velocity, distanceNormal));
	Vec3 c2Proj = vec3_scale(distanceNormal, vec3_dot(c2->velocity, distanceNormal));

	Vec3 c1PrevVelocity = c1->velocity;
	Vec3 c2PrevVelocity = c2->velocity;

	c1->velocity = vec3_subtract(c1->velocity, c1Proj);
	c1->velocity = vec3_sum(c1->velocity, c2Proj);

	c2->velocity = vec3_subtract(c2->velocity, c2Proj);
	c2->velocity = vec3_sum(c2->velocity, c1Proj);

	c1->position = vec3_sum(c1->position, (Vec3) { -1 * delta.x, -1 * delta.y, -1 * delta.z });
	c2->position = vec3_sum(c2->position, delta);


	CollisionInfo ci;

	ci.otherCollider = c2;
	ci.collisionForce = vec3_scale(vec3_subtract(c1->velocity, c1PrevVelocity), 1.0f / DELTA_TIME);
	seqtor_push_back(c1->collisions, ci);

	ci.otherCollider = c1;
	ci.collisionForce = vec3_scale(vec3_subtract(c2->velocity, c2PrevVelocity), 1.0f / DELTA_TIME);
	seqtor_push_back(c2->collisions, ci);

	return 1;
}

Vec3 physics_closestPointOfLineSegment(Vec3 point, Vec3 lineA, Vec3 lineB)
{
	Vec3 delta = (Vec3){ point.x - lineA.x,point.y - lineA.y,point.z - lineA.z };
	Vec3 lineDir = vec3_normalize((Vec3) { lineB.x - lineA.x, lineB.y - lineA.y, lineB.z - lineA.z });

	Vec3 projectionToLine = vec3_sum(lineA, vec3_scale(lineDir, vec3_dot(delta, lineDir)));

	//is the point before lineA?
	if (vec3_dot(lineDir, (Vec3) { projectionToLine.x - lineA.x, projectionToLine.y - lineA.y, projectionToLine.z - lineA.z })<0)
	{
		projectionToLine = lineA;
	}
	//is the point over lineB?
	else if (vec3_sqrMagnitude((Vec3) { projectionToLine.x - lineA.x, projectionToLine.y - lineA.y, projectionToLine.z - lineA.z }) > vec3_sqrMagnitude((Vec3) { lineB.x - lineA.x, lineB.y - lineA.y, lineB.z - lineA.z }))
	{
		projectionToLine = lineB;
	}

	return projectionToLine;
}