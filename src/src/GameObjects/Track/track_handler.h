#ifndef TRACK_HANDLER_H
#define TRACK_HANDLER_H

#include "../../Glm2/mat4.h"

#define TH_MAX_SEGMENT_COUNT 50
#define TH_SEGMENT_LENGTH 10
#define TH_SEGMENT_RESOLUTION 5

void* trackHandler_create();
void trackHandler_destroy(void* trackHandler);

void trackHandler_update(void* trackHandler, float deltaTime);
void trackHandler_onStart(void* trackHandler);
void trackHandler_onDestroy(void* trackHandler);

void trackHandler_render(void* trackHandler);

#endif