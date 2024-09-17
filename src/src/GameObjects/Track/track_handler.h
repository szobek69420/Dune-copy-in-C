#ifndef TRACK_HANDLER_H
#define TRACK_HANDLER_H

#include "../../Glm2/mat4.h"

void* trackHandler_create();
void trackHandler_destroy(void* trackHandler);

void trackHandler_update(void* trackHandler, float deltaTime);
void trackHandler_onStart(void* trackHandler);
void trackHandler_onDestroy(void* trackHandler);

void trackHandler_render(void* trackHandler);

#endif