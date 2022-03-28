#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "util.h"

struct Camera
{
    // Stores the main vectors of the camera
    vec3 position;
    vec3 orientation;
    vec3 up;

    // Projection matrix
    mat4 projection;

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick;

    // Stores the width and height of the window
    unsigned int width;
    unsigned int height;

    // Adjust the speed of the camera and it's sensitivity when looking around
    float speed;
    float sensitivity;
};

struct Camera Camera_Create(unsigned int width, unsigned int height, vec3 position, float FOV_deg, float near_plane, float far_plane);
void Camera_View_Projection_To_Shader(struct Camera camera, const struct Shader shader, const char *uniform);
void Camera_Inputs(struct Camera *camera);

#endif // __CAMERA_H__