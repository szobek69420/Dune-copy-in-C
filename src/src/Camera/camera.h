#ifndef CAMERA_H
#define CAMERA_H

#include "../Glm2/vec3.h"
#include "../Glm2/mat4.h"

struct Camera;
typedef struct Camera Camera;

struct Camera* camera_create(struct Vec3 position, struct Vec3 world_up);
void camera_destroy(struct Camera* cum);
void camera_setProjection(struct Camera* cum, float width, float height, float near_plane, float far_plane);
void camera_setForward(struct Camera* cum, struct Vec3 forward);

struct Mat4 camera_getViewMatrix(struct Camera* cum);
struct Mat4 camera_getProjectionMatrix(struct Camera* cum);

#endif