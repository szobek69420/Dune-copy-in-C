#ifndef CAMERA_H
#define CAMERA_H

#include "../Glm2/vec3.h"
#include "../Glm2/mat4.h"

void* camera_create(struct Vec3 position, struct Vec3 world_up, float mouse_sensitivity);
void camera_destroy(void* cum);
void camera_setProjection(void* cum, float fov, float aspectXY, float near_plane, float far_plane);
void camera_updateVectors(void* cum);

struct Mat4 camera_getViewMatrix(void* cum);

#endif