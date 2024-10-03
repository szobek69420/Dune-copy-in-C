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
    float width, height;
    float near_plane, far_plane;
    struct Mat4 view_matrix;
    struct Mat4 projection_matrix;
};
typedef struct Camera Camera;

void camera_updateVectors(struct Camera* cum);


Camera* camera_create(struct Vec3 position, struct Vec3 world_up)
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
    cam->width = 10;
    cam->height = 5;
    cam->near_plane = 0.1;
    cam->far_plane = 300;
    camera_updateVectors(&cam);
    return cam;
}

void camera_destroy(struct Camera* cum)
{
    free(cum);
}

void camera_setProjection(struct Camera* cum, float width, float height, float near_plane, float far_plane)
{
    Camera* cam = (Camera*)cum;
    cam->width = width;
    cam->height = height;
    cam->near_plane = near_plane;
    cam->far_plane = far_plane;
    cam->projection_matrix = mat4_ortho(-0.5f*width,0.5f*width,-0.5f*height,0.5f*height,near_plane, far_plane);
}


Mat4 camera_getViewMatrix(struct Camera* cum)
{
    return ((Camera*)cum)->view_matrix;
}

Mat4 camera_getProjectionMatrix(struct Camera* cum)
{
    return cum->projection_matrix;
}


#pragma warning( push )
#pragma warning( disable : 4789 )

void camera_updateVectors(struct Camera* cum)
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

void camera_setForward(struct Camera* cam, struct Vec3 forward)
{
    cam->front = vec3_normalize(forward);
    cam->right = vec3_normalize(vec3_cross(cam->front, cam->world_up));
    cam->up = vec3_normalize(vec3_cross(cam->right, cam->front));
    cam->view_matrix = mat4_lookAt(cam->position, cam->front, cam->up);
}

void camera_setPosition(struct Camera* cum, struct Vec3 position)
{
    cum->position = position;
    cum->view_matrix = mat4_lookAt(cum->position, cum->front, cum->up);
}

struct Vec3 camera_getPosition(struct Camera* cum)
{
    return cum->position;
}

float camera_getWidth(struct Camera* cum)
{
    return cum->width;
}

float camera_getHeight(struct Camera* cum)
{
    return cum->height;
}