#ifndef PLAYER_H
#define PLAYER_H

#include "../../Glm2/mat4.h"

void* player_create();
void player_destroy(void* player);

void player_update(void* player, float deltaTime);
void player_onStart(void* player);
void player_onDestroy(void* player);

void player_render(void* player, struct Mat4 parentModel);

#endif
