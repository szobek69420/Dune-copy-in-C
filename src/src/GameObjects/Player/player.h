#ifndef PLAYER_H
#define PLAYER_H

#include "../game_object.h"
#include "../../Renderer/renderer.h"
#include "../../Glm2/mat4.h"

struct Player {
	Transform transform;
	Renderable renderable;
};
typedef struct Player Player;

void* player_create();
void player_destroy(void* player);

void player_update(void* player, float deltaTime);
void player_onStart(void* player);
void player_onDestroy(void* player);

void player_render(void* player, struct Mat4 parentModel);

#endif
