#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "track_handler.h"

#include "../game_object.h"
#include "../../Physics/physics.h"
#include "../../Renderer/renderer.h"
#include "../../Glm2/mat4.h"
#include "../../Glm2/vec3.h"
#include "../../Glm2/quaternion.h"

#define SEGMENT_VERTEX_COUNT (2*(TH_SEGMENT_RESOLUTION*TH_SEGMENT_LENGTH+1))
#define SEGMENT_INDEX_COUNT (3*(SEGMENT_VERTEX_COUNT-2))

struct TrackSegment;
typedef struct TrackSegment TrackSegment;

struct TrackHandler 
{
	Transform transform;

	seqtor_of(TrackSegment*) segments;
	Vec3 currentSegmentStart;

	void* player;
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
	trackHandler->currentSegmentStart = (Vec3){ 0,0,0 };
	trackHandler->player = NULL;

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
	TrackHandler* th = trackHandler;

	if (seqtor_size(th->segments) < TH_MAX_SEGMENT_COUNT)
	{
		TrackSegment* tsz = createSegment(th->currentSegmentStart);
		seqtor_push_back(th->segments, tsz);
		th->currentSegmentStart = vec3_sum(th->currentSegmentStart, (Vec3) { TH_SEGMENT_LENGTH, 0, 0 });
	}
	else
	{
		if (th->player == NULL)
			th->player = gameObject_getByName("player");
		else if (gameObject_isAlive(th->player) == 0)
			th->player = NULL;

		if (th->player != NULL && th->currentSegmentStart.x - TH_MAX_SEGMENT_COUNT * TH_SEGMENT_LENGTH + 40 < ((TrackHandler*)th->player)->transform.position.x)
		{
			destroySegment(seqtor_at(th->segments, 0));
			seqtor_remove_at(th->segments, 0);

			TrackSegment* tsz = createSegment(th->currentSegmentStart);
			seqtor_push_back(th->segments, tsz);
			th->currentSegmentStart = vec3_sum(th->currentSegmentStart, (Vec3) { TH_SEGMENT_LENGTH, 0, 0 });
		}
	}
}

void trackHandler_onStart(void* trackHandler)
{
	TrackHandler* th = trackHandler;

	th->transform.position = (Vec3){ 0,0,0 };
	th->transform.rotation = quat_init();
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


float mapGenerator(float x)
{
	return 10.0f +5 * (1-sqrtf(powf(sinf(0.15f*x),2.0f)));
}

TrackSegment* createSegment(Vec3 position)
{
	TrackSegment* tsz = malloc(sizeof(TrackSegment));

	tsz->position = position;


	float* vertices = malloc(5*SEGMENT_VERTEX_COUNT * sizeof(float));
	unsigned int* indices = malloc(SEGMENT_INDEX_COUNT * sizeof(unsigned int));

	float deltaX = 1.0f / TH_SEGMENT_RESOLUTION;
	float currentX = tsz->position.x;

	float deltaUVX= 1.0f / (TH_SEGMENT_LENGTH*TH_SEGMENT_RESOLUTION);
	float currentUVX = 0;

	for (int i = 0,j=0,k=0; i <SEGMENT_VERTEX_COUNT/2; i++,j+=5, currentX += deltaX,currentUVX+=deltaUVX)
	{
		//vertices
		vertices[j] = currentX-tsz->position.x;
		vertices[j + 1] = mapGenerator(currentX);
		vertices[j + 2] = 0;
		vertices[j + 3] = currentUVX;
		vertices[j + 4] = 1.0f;

		j += 5*(SEGMENT_VERTEX_COUNT/2);
		vertices[j] = currentX- tsz->position.x;
		vertices[j + 1] = 0;
		vertices[j + 2] = 0;
		vertices[j + 3] = currentUVX;
		vertices[j + 4] = 0;

		j-= 5 * (SEGMENT_VERTEX_COUNT / 2);

		//indices
		if (i == 0)//first tile starts at i==1
			continue;

		indices[k++] = i - 1;
		indices[k++] = i + TH_SEGMENT_RESOLUTION * TH_SEGMENT_LENGTH + 1;
		indices[k++] = i;
		indices[k++] = i - 1;
		indices[k++] = i + TH_SEGMENT_RESOLUTION * TH_SEGMENT_LENGTH;
		indices[k++] = i + TH_SEGMENT_RESOLUTION * TH_SEGMENT_LENGTH + 1;
	}

	tsz->renderable = renderer_createRenderable(vertices, 5*SEGMENT_VERTEX_COUNT, indices, SEGMENT_INDEX_COUNT,0);
	tsz->renderable.texture = TEXTURE;


	Vec3* colliderPoints = malloc(sizeof(Vec3) * (SEGMENT_VERTEX_COUNT + 1));
	for (int i = 0,j=0; i < SEGMENT_VERTEX_COUNT / 2; i++,j+=5)
		colliderPoints[i] = *(Vec3*)(vertices + j);
	for (int i = 5 * (SEGMENT_VERTEX_COUNT - 1), j = SEGMENT_VERTEX_COUNT / 2; i >= 5 * (SEGMENT_VERTEX_COUNT / 2); i -= 5, j++)
		colliderPoints[j] = *(Vec3*)(vertices + i);
	colliderPoints[SEGMENT_VERTEX_COUNT] = *(Vec3*)vertices;


	tsz->collider = physics_createPolygonCollider(colliderPoints, SEGMENT_VERTEX_COUNT+1);
	physics_setColliderParam(tsz->collider, POSITION_VEC3, &position);
	int isMovable = 0;
	physics_setColliderParam(tsz->collider, MOVABLE_INT, &isMovable);

	free(vertices);
	free(indices);
	free(colliderPoints);

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
	renderer_setRenderMode(GL_TRIANGLES);
	
	renderer_renderObject(segment->renderable, mat4_multiply(parentModel, mat4_translate(mat4_create(1), segment->position)));
}