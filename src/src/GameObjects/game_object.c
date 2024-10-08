#define _CRT_SECURE_NO_WARNINGS

#include <seqtor.h>
#include <stdint.h>
#include <string.h>
#include "game_object.h"
#include "Player/player.h"
#include "Track/track_handler.h"
#include "Background/background.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define RAD2DEG 57.2957795f

static uint32_t currentId = 69;
int gameObject_getNextId() { return currentId++; }

typedef struct RootObject {
	Transform transform;
	GameObjectFunctions functions;
} RootObject;

RootObject* root;
seqtor_of(void*) REGISTERED_GAMEOBJECTS;//root is not part of it


void gameObject_init()
{
	seqtor_init(REGISTERED_GAMEOBJECTS, 1);
	root = (RootObject*)malloc(sizeof(RootObject));
	root->transform = gameObject_createTransform("root");
	memset(&root->functions, 0, sizeof(GameObjectFunctions));
	root->transform.isInitialized = 69;
}

void gameObject_deinit()
{
	gameObject_destroy(root);
	seqtor_destroy(REGISTERED_GAMEOBJECTS);
}


void _updateHelper(void* gameObject, float deltaTime);
void gameObject_update(float deltaTime)
{
	for (int i = 0; i < seqtor_size(root->transform.children); i++)
		_updateHelper(seqtor_at(root->transform.children, i), deltaTime);
}

void gameObject_onStart(void* gameObject)
{
	RootObject* go = gameObject;

	if (go->functions.onStart != NULL)
		go->functions.onStart(go);

	go->transform.isInitialized = 69;
}
void gameObject_onDestroy(void* gameObject)
{
	RootObject* go = gameObject;

	if (go->functions.onDestroy != NULL)
		go->functions.onDestroy(go);
}

void _updateHelper(void* gameObject, float deltaTime)
{
	RootObject* p = (RootObject*)gameObject;
	if (p->transform.isInitialized == 0)
	{
		gameObject_onStart(gameObject);
		return;
	}

	if (p->functions.onUpdate != NULL)
		p->functions.onUpdate(p,deltaTime);

	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		_updateHelper(seqtor_at(p->transform.children, i), deltaTime);
}

void _renderHelper(void* gameObject);
void gameObject_render(void* pwindow)
{
	for (int i = 0; i < seqtor_size(root->transform.children); i++)
		_renderHelper(seqtor_at(root->transform.children, i));
}

void _renderHelper(void* gameObject)
{
	RootObject* p = (RootObject*)gameObject;

	if (p->functions.render != NULL)
		p->functions.render(p);

	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		_renderHelper(seqtor_at(p->transform.children, i));
}


void* gameObject_create(GameObjects type,const char* name)
{
	RootObject* gameObject = NULL;
	switch (type)
	{
	case PLAYER:
		gameObject=player_create(name);
		break;

	case TRACK_HANDLER:
		gameObject = trackHandler_create(name);
		break;

	case BACKGROUND:
		gameObject = background_create(name);
		break;
	}

	return gameObject;
}

void gameObject_add(void* gameObject, void* parent)
{
	for (int i = 0; i < seqtor_size(REGISTERED_GAMEOBJECTS); i++)
	{
		if (gameObject == seqtor_at(REGISTERED_GAMEOBJECTS, i))
		{
			fprintf(stderr, "you cannot register the same game object twice\n");
			return;
		}
	}

	RootObject* p = (RootObject*)parent;
	if (p == NULL)
	{
		seqtor_push_back(root->transform.children, gameObject);
		((RootObject*)gameObject)->transform.parent = &(root->transform);
	}
	else
	{
		seqtor_push_back(((RootObject*)parent)->transform.children, gameObject);
		((RootObject*)gameObject)->transform.parent = &(p->transform);
	}

	seqtor_push_back(REGISTERED_GAMEOBJECTS, gameObject);
}

void gameObject_removeAll()
{
	while (seqtor_size(root->transform.children) > 0)
	{
		RootObject* child = seqtor_at(root->transform.children, 0);
		seqtor_remove_at(root->transform.children, 0);
		gameObject_destroy(child);
	}
}

void gameObject_destroy(void* gameObject)
{
	RootObject* p = (RootObject*)gameObject;
	printf("GameObject: destroying %s\n", p->transform.name);

	gameObject_onDestroy(gameObject);//azert van ez a rekurziv hivas elott, mert azt akarom, hogy a szulo onDestroy-a elobb fusson le, mint a gyerekeie


	while (seqtor_size(p->transform.children) > 0)//a gyerekek destroyat elobb akarom meghivni, mint a szuloet
	{
		RootObject* child = seqtor_at(p->transform.children, 0);
		seqtor_remove_at(p->transform.children, 0);
		gameObject_destroy(child);
	}

	if (p->functions.destroy != NULL)
		p->functions.destroy(p);
	else
	{
		gameObject_destroyTransform(&(p->transform));
		free(p);
	}


	if (gameObject != root)
	{
		void* parent = gameObject_getParent(gameObject);
		if (parent == NULL)//meg lehet a root gyereke
			parent = root;
		seqtor_remove(((RootObject*)parent)->transform.children, gameObject);
	}

	seqtor_remove(REGISTERED_GAMEOBJECTS, gameObject);
}

void* getByNameHelper(void* gameObject, const char* name)
{
	RootObject* go = gameObject;

	if (strcmp(name, go->transform.name) == 0)
		return go;

	for (int i = 0; i < seqtor_size(go->transform.children); i++)
	{
		void* temp = getByNameHelper(seqtor_at(go->transform.children, i), name);
		if (temp != NULL)
			return temp;
	}

	return NULL;
}
void* gameObject_getByName(const char* name)
{
	if (name == NULL || strcmp(name, "root") == 0)
		return NULL;

	for (int i = 0; i < seqtor_size(REGISTERED_GAMEOBJECTS); i++)
	{
		if (strcmp(((RootObject*)seqtor_at(REGISTERED_GAMEOBJECTS, i))->transform.name, name) == 0)
			return seqtor_at(REGISTERED_GAMEOBJECTS, i);
	}
	return NULL;
}

int gameObject_isAlive(void* gameObject)
{
	for (int i = 0; i < seqtor_size(REGISTERED_GAMEOBJECTS); i++)
	{
		if (seqtor_at(REGISTERED_GAMEOBJECTS, i) == gameObject)
			return 69;
	}
	return 0;
}

void* getParentHelper(void* currentGameObject,void* searched)
{
	if (currentGameObject == searched)
		return NULL;

	RootObject* go = currentGameObject;
	for (int i = 0; i < seqtor_size(go->transform.children); i++)
	{
		if (seqtor_at(go->transform.children, i) == searched)
			return currentGameObject;
	}

	for (int i = 0; i < seqtor_size(go->transform.children); i++)
	{
		void* temp = getParentHelper(seqtor_at(go->transform.children, i), searched);
		if (temp != NULL)
			return temp;
	}

	return NULL;
}
void* gameObject_getParent(void* gameObject)
{
	void* parent = getParentHelper(root, gameObject);
	if (parent == root)
		return NULL;
	return parent;
}

void gameObject_setParent(void* gameObject, void* parent)
{
	if (gameObject == parent)
	{
		printf("Parent cannot be the object itself\n");
		return;
	}

	//kiszedni a korabbi szulotol
	RootObject* prevParent = gameObject_getParent(gameObject);
	if (prevParent == NULL)
		prevParent = root;
	seqtor_remove(prevParent->transform.children, gameObject);

	//hozzaadni az uj szulohoz
	seqtor_push_back(((RootObject*)parent)->transform.children, gameObject);
	((RootObject*)gameObject)->transform.parent = &((RootObject*)parent)->transform;
}

Transform gameObject_createTransform(const char* name)
{
	Transform transform;
	transform.position = (Vec3){ 0,0,0 };
	transform.rotation = quat_init();

	seqtor_init(transform.children, 1);
	transform.isInitialized = 0;
	transform.id = gameObject_getNextId();

	transform.parent = NULL;

	if (name != NULL)
	{
		transform.name = malloc((strlen(name) + 1) * sizeof(char));
		strcpy(transform.name, name);
	}
	else
	{
		char buffer[25];
		sprintf(buffer, "gameobject #%d", transform.id);
		transform.name = malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(transform.name, buffer);
	}
	return transform;
}

void gameObject_destroyTransform(Transform* transform)
{
	free(transform->name);
	seqtor_destroy(transform->children);
}

Mat4 gameObject_getTransformModel(const Transform* transform)
{
	Mat4 model = mat4_create(1);
	model = mat4_translate(model, transform->position);
	model = mat4_multiply(model,quat_rotationMatrix(transform->rotation));
	return model;
}

Mat4 gameObject_getTransformModelInverse(const Transform* transform)
{
	Mat4 inverse = quat_rotationMatrix((Quat) { transform->rotation.s, -transform->rotation.x, -transform->rotation.y, -transform->rotation.z });
	inverse = mat4_translate(inverse, (Vec3) { -transform->position.x, -transform->position.y, -transform->position.z });
	return inverse;
}

Mat4 gameObject_getTransformWorldModel(const Transform* transform)
{
	Mat4 model = mat4_create(1);
	do {
		model = mat4_multiply(gameObject_getTransformModel(transform), model);
		transform = transform->parent;
	} while (transform != NULL);

	return model;
}

Mat4 gameObject_getTransformWorldModelInverse(const Transform* transform)
{
	Mat4 inverse = mat4_create(1);
	do {
		inverse = mat4_multiply(inverse, gameObject_getTransformModelInverse(transform));
		transform = transform->parent;
	} while (transform != NULL);

	return inverse;
}


Vec3 gameObject_getWorldPosition(void* gameObject)
{
	Transform* transform= &(((RootObject*)gameObject)->transform);
	Mat4 model = gameObject_getTransformWorldModel(transform);

	Vec4 temp = vec4_create2(transform->position.x, transform->position.y, transform->position.z, 1);
	temp = vec4_multiplyWithMatrix(model, temp);
	return (Vec3) { temp.x, temp.y, temp.z };
}

void gameObject_setWorldPosition(void* gameObject, Vec3 position)
{
	Mat4 modelInverse = gameObject_getTransformWorldModelInverse((const Transform*)&(((RootObject*)gameObject)->transform));
	Vec4 temp = vec4_multiplyWithMatrix(modelInverse, (Vec4) { position.x, position.y, position.z, 1 });
	((RootObject*)gameObject)->transform.position = *(Vec3*)&temp;
}
