#include <seqtor.h>
#include <stdint.h>
#include "game_object.h"

static uint32_t currentId = 69;
int gameObject_getNextId() { return currentId++; }

typedef struct RootObject {
	Transform transform;
} RootObject;

RootObject* root;

int gameObject_getType(void* go)
{
	return ((RootObject*)go)->transform.type;
}

void _updateHelper(void* gameObject, float deltaTime);
void gameObject_update(float deltaTime)
{
	for (int i = 0; i < seqtor_size(root->transform.children); i++)
		_updateHelper(seqtor_at(root->transform.children, i), deltaTime);
}

void _updateHelper(void* gameObject, float deltaTime)
{
	RootObject* p = (RootObject*)gameObject;

	switch (p->transform.type)
	{
	case PLAYER:

		break;

	case TRACK_HANDLER:

		break;
	}

	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		_updateHelper(seqtor_at(p->transform.children, i), deltaTime);
}

void gameObject_render(void* pwindow);

void gameObject_init()
{
	root = (RootObject*)malloc(sizeof(RootObject));
	root->transform.id = gameObject_getNextId();
	root->transform.type = -1;
	seqtor_init(root->transform.children, 1);
}

void gameObject_end()
{
	gameObject_destroy(root);
}

void gameObject_destroy(void* gameObject)
{
	RootObject* p = (RootObject*)gameObject;
	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		gameObject_destroy(seqtor_at(p->transform.children, i));

	switch (p->transform.type)
	{
	case PLAYER:

		break;

	case TRACK_HANDLER:

		break;

	default:
		seqtor_destroy(p->transform.children);
		free(p);
		break;
	}
}
