#include <stdlib.h>
#include <stdio.h>

#include "../game_object.h"
#include "../../Physics/physics.h"
#include "../../Renderer/renderer.h"
#include "../../Glm2/mat4.h"
#include "../../Glm2/vec3.h"
#include "../../Glm2/quaternion.h"

struct TrackSegment;
typedef struct TrackSegment TrackSegment;

struct TrackHandler 
{
	Transform transform;
	seqtor_of(TrackSegment*) segments;
};
typedef struct TrackHandler TrackHandler;

struct TrackSegment
{
	Vec3 position;
	Renderable renderable;
	Collider* collider;
};

texture_t TEXTURE = 0;


TrackSegment* createSegment(Vec3 position);
void destroySegment(TrackSegment* segment);
void renderSegment(TrackSegment* segment, Mat4 parentModel);


void* trackHandler_create()
{
	TrackHandler* trackHandler = malloc(sizeof(TrackHandler));
	seqtor_init(trackHandler->segments, 1);

	TEXTURE = renderer_createTexture("Assets/Sprites/track.png", 4);

	return trackHandler;
}

void trackHandler_destroy(void* trackHandler)
{
	TrackHandler* th = trackHandler;

	seqtor_destroy(th->segments);

	renderer_destroyTexture(TEXTURE);

	free(th);
}

void trackHandler_update(void* trackHandler, float deltaTime)
{

}

void trackHandler_onStart(void* trackHandler)
{
	TrackHandler* th = trackHandler;

	th->transform.position = (Vec3){ 0,0,0 };
	th->transform.rotation = quat_init();

	TrackSegment* tsz = createSegment((Vec3) { 0, 0, 0 });
	seqtor_push_back(th->segments, tsz);
}

void trackHandler_onDestroy(void* trackHandler)
{
	TrackHandler* th = trackHandler;

	for (int i = 0; i < seqtor_size(th->segments); i++)
		destroySegment(seqtor_at(th->segments, i));
	seqtor_clear(th->segments);
}

void trackHandler_render(void* trackHandler)
{
	TrackHandler* th = trackHandler;

	Mat4 model = gameObject_getTransformWorldModel(&th->transform);
	for (int i = 0; i < seqtor_size(th->segments); i++)
		renderSegment(seqtor_at(th->segments, i), model);
}


TrackSegment* createSegment(Vec3 position)
{
	TrackSegment* tsz = malloc(sizeof(TrackSegment));

	tsz->position = position;

	tsz->renderable = renderer_createRenderable((float[]) { -1, -1, 0, 0, 0, -1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, -1, 0, 1, 0 }, 20, NULL, 4);
	tsz->renderable.texture = TEXTURE;

	tsz->collider = physics_createPolygonCollider((Vec3[]) { { -1, -1, 0 }, { -1,1,0 }, { 1,1,0 }, { 1,-1,0 }, { -1,-1,0 } }, 5);
	physics_setColliderParam(tsz->collider, POSITION_VEC3, &position);
	int isMovable = 69;
	physics_setColliderParam(tsz->collider, MOVABLE_INT, &isMovable);

	return tsz;
}

void destroySegment(TrackSegment* segment)
{
	renderer_destroyRenderable(segment->renderable);
	physics_destroyCollider(segment->collider);
	free(segment);
}

void renderSegment(TrackSegment* segment, Mat4 parentModel)
{
	renderer_useShader(DEFAULT_SHADER);
	renderer_setRenderMode(GL_TRIANGLE_FAN);
	renderer_renderObject(segment->renderable, mat4_multiply(parentModel, mat4_translate(mat4_create(1), segment->position)));
}