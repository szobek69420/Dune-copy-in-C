#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "../Glm2/vec3.h"
#include "../Glm2/quaternion.h"

/*every gameobject's first field is a Transform called "transform"
* and the struct definition should be in the .c file
* */

struct Transform {
	uint32_t id;
	uint32_t type;
	seqtor_of(void*) children;
	struct Vec3 position;
	struct Quaternion rotation;
	struct Vec3 scale;
};
typedef struct Transform Transform;

enum GameObjects{
	TRACK_HANDLER,
	PLAYER
};

int gameObject_getNextId();

void gameObject_update(float deltaTime);
void gameObject_render(void* pwindow);

void gameObject_init();
void gameObject_end();

//if parent is NULL, then it goes under the root component
void gameObject_add(void* gameObject, void* parent);
//destroys children as well
void gameObject_destroy(void* gameObject);

#endif
