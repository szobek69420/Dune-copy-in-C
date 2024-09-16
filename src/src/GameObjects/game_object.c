#define _CRT_SECURE_NO_WARNINGS

#include <seqtor.h>
#include <stdint.h>
#include <string.h>
#include "game_object.h"
#include "Player/player.h"

#define RAD2DEG 57.2957795f

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

void gameObject_onStart(void* gameObject)
{
	switch (((RootObject*)gameObject)->transform.type)
	{
	case PLAYER:
		player_onStart(gameObject);
		break;

	case TRACK_HANDLER:

		break;
	}

	((RootObject*)gameObject)->transform.isInitialized = 69;
}
void gameObject_onDestroy(void* gameObject)
{
	switch (((RootObject*)gameObject)->transform.type)
	{
	case PLAYER:
		player_onDestroy(gameObject);
		break;

	case TRACK_HANDLER:

		break;
	}
}

void _updateHelper(void* gameObject, float deltaTime)
{
	RootObject* p = (RootObject*)gameObject;
	if (p->transform.isInitialized == 0)
	{
		gameObject_onStart(gameObject);
		return;
	}

	switch (p->transform.type)
	{
	case PLAYER:
		player_update(gameObject, deltaTime);
		break;

	case TRACK_HANDLER:

		break;
	}

	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		_updateHelper(seqtor_at(p->transform.children, i), deltaTime);
}

void _renderHelper(void* gameObject, Mat4 parentModel);
void gameObject_render(void* pwindow)
{
	for (int i = 0; i < seqtor_size(root->transform.children); i++)
		_renderHelper(seqtor_at(root->transform.children, i), mat4_create(1));
}

void _renderHelper(void* gameObject, Mat4 parentModel)
{
	RootObject* p = (RootObject*)gameObject;

	switch (p->transform.type)
	{
	case PLAYER:
		player_render(gameObject,parentModel);
		break;

	case TRACK_HANDLER:

		break;
	}

	Mat4 model = mat4_multiply(parentModel, gameObject_getTransformModel(&(p->transform)));

	for (int i = 0; i < seqtor_size(p->transform.children); i++)
		_renderHelper(seqtor_at(p->transform.children, i),model);
}

void gameObject_init()
{
	root = (RootObject*)malloc(sizeof(RootObject));
	root->transform = gameObject_createTransform(-1,"root");
	root->transform.isInitialized = 69;

	void* player = NULL;
	player = gameObject_create(PLAYER,"player");
	gameObject_add(player, NULL);
}

void gameObject_deinit()
{
	gameObject_destroy(root);
}

void* gameObject_create(GameObjects type,const char* name)
{
	RootObject* gameObject = NULL;
	switch (type)
	{
	case PLAYER:
		gameObject=player_create();
		gameObject->transform=gameObject_createTransform(type, name);
		break;

	case TRACK_HANDLER:

		break;
	}

	return gameObject;
}

void gameObject_add(void* gameObject, void* parent)
{
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
}

void gameObject_destroy(void* gameObject)
{
	RootObject* p = (RootObject*)gameObject;

	gameObject_onDestroy(gameObject);//azert van ez a rekurziv hivas elott, mert azt akarom, hogy a szulo onDestroy-a elobb fusson le, mint a gyerekeie


	for (int i = 0; i < seqtor_size(p->transform.children); i++)//a gyerekek destroyat elobb akarom meghivni, mint a szuloet
		gameObject_destroy(seqtor_at(p->transform.children, i));

	switch (p->transform.type)
	{
	case PLAYER:
		player_destroy(gameObject);
		break;

	case TRACK_HANDLER:

		break;

	default:
		gameObject_destroyTransform(&(p->transform));
		free(p);
		break;
	}


	if (gameObject != root)
	{
		void* parent = gameObject_getParent(gameObject);
		if (parent == NULL)//meg lehet a root gyereke
			parent = root;
		seqtor_remove(((RootObject*)parent)->transform.children, gameObject);
	}
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

	return getByNameHelper(root, name);
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

Transform gameObject_createTransform(GameObjects type, const char* name)
{
	Transform transform;
	transform.position = (Vec3){ 0,0,0 };
	transform.rotation = quat_init();

	seqtor_init(transform.children, 1);
	transform.isInitialized = 0;
	transform.id = gameObject_getNextId();
	transform.type = type;

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
