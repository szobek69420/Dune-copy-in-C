#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <seqtor.h>
#include <stdint.h>

#include "../Glm2/vec3.h"
#include "../Glm2/quaternion.h"
#include "../Glm2/mat4.h"

/*every gameobject's first field is a Transform
* the second field is a GameObjectFunctions
* and the struct definition should be in the .c file
* */

struct Transform {
	char* name;
	uint32_t id;
	seqtor_of(void*) children;
	struct Vec3 position;
	struct Quaternion rotation;
	int isInitialized;
	const struct Transform* parent;
};
typedef struct Transform Transform;

struct GameObjectFunctions {
	void (*onStart)(void*);
	void (*onUpdate)(void*, float);//go, dt
	void (*onDestroy)(void*);
	void (*destroy)(void*);
	void (*render)(void*);
};
typedef struct GameObjectFunctions GameObjectFunctions;

enum GameObjects{
	TRACK_HANDLER,
	PLAYER,
	BACKGROUND
};
typedef enum GameObjects GameObjects;

void gameObject_update(float deltaTime);
void gameObject_onStart(void* gameObject);
void gameObject_onDestroy(void* gameObject);

void gameObject_render(void* pwindow);

void gameObject_init();
void gameObject_deinit();

void* gameObject_create(GameObjects type,const char* name);

//if parent is NULL, then it goes under the root component
void gameObject_add(void* gameObject, void* parent);
void gameObject_removeAll();
//destroys children as well
void gameObject_destroy(void* gameObject);

void* gameObject_getByName(const char* name);
int gameObject_isAlive(void* gameObject);

void* gameObject_getParent(void* gameObject);//it cannot return the root component
void gameObject_setParent(void* gameObject, void* parent);

Transform gameObject_createTransform(const char* name);
void gameObject_destroyTransform(Transform* transform);
Mat4 gameObject_getTransformModel(const Transform* transform);
Mat4 gameObject_getTransformModelInverse(const Transform* transform);
Mat4 gameObject_getTransformWorldModel(const Transform* transform);
Mat4 gameObject_getTransformWorldModelInverse(const Transform* transform);

Vec3 gameObject_getWorldPosition(void* gameObject);
void gameObject_setWorldPosition(void* gameObject, Vec3 position);

#endif
