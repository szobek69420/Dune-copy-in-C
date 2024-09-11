#include "camera.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../Glm2/vec3.h"
#include "../Glm2/mat4.h"

#define DEG2RAD 0.01745329251f

struct Camera {
    struct Vec3 position;
    struct Vec3 front, up, right;
    struct Vec3 world_up;
    float yaw, pitch;
    float fov, aspectXY;
    float near_plane, far_plane;
    float mouse_sensitivity;
    struct Mat4 view_matrix;
    struct Mat4 projection_matrix;
};
typedef struct Camera Camera;

void* camera_create(struct Vec3 position, struct Vec3 world_up, float mouse_sensitivity)
{
    Camera* cam=(Camera*)malloc(sizeof(Camera));
    if (cam == NULL)
    {
        fprintf(stderr, "Camera: camera could not be created");
        return NULL;
    }
    cam->position = position;
    cam->world_up = world_up;
    cam->yaw = 0;
    cam->pitch = 0;
    cam->fov = 60;
    cam->aspectXY = 1;
    cam->near_plane = 0.1;
    cam->far_plane = 300;
    cam->mouse_sensitivity = mouse_sensitivity;
    camera_updateVectors(&cam);
    return cam;
}

void camera_destroy(void* cum)
{
    free(cum);
}

void camera_setProjection(void* cum, float fov, float aspectXY, float near_plane, float far_plane)
{
    Camera* cam = (Camera*)cum;
    cam->fov = fov;
    cam->near_plane = near_plane;
    cam->far_plane = far_plane;
    cam->aspectXY = aspectXY;
    cam->projection_matrix = mat4_perspective(cam->fov, cam->aspectXY, cam->near_plane, cam->far_plane);
}


Mat4 camera_getViewMatrix(void* cum)
{
    return ((Camera*)cum)->view_matrix;
}


#pragma warning( push )
#pragma warning( disable : 4789 )

void camera_updateVectors(void* cum)
{
    Camera* cam = (Camera*)cum;
    Vec3 front;
    front.x = -sinf(cam->yaw * DEG2RAD) * cosf(cam->pitch * DEG2RAD);
    front.y = sinf(cam->pitch * DEG2RAD);
    front.z = -cosf(cam->yaw * DEG2RAD) * cosf(cam->pitch * DEG2RAD);
    cam->front = vec3_normalize(front);
    cam->right = vec3_normalize(vec3_cross(cam->front, cam->world_up));
    cam->up = vec3_normalize(vec3_cross(cam->right, cam->front));
    cam->view_matrix = mat4_lookAt(cam->position, cam->front, cam->up);
}

#pragma warning( pop )