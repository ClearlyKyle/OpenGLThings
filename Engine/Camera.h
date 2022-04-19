#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "util.h"
#include "Shaders.h"
#include "Window.h"

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

    int lastX;
    int lastY;

    float yaw;
    float pitch;

    // Adjust the speed of the camera and it's sensitivity when looking around
    float speed;
    float sensitivity;
};

typedef struct Camera Camera_t;

Camera_t Camera_Create(unsigned int width, unsigned int height, vec3 position, float FOV_deg, float near_plane, float far_plane);
void Camera_View_Projection_To_Shader(struct Camera camera, const struct Shader shader, const char *uniform);

void Camera_Get_View_Matrix(struct Camera camera, mat4 view_matrix);
void Camera_Set_Orientation(struct Camera *camera, vec3 new_orientation);
void Camera_Set_Position(struct Camera *camera, vec3 new_position);

void Camera_Inputs(struct Camera *camera);
void Camera_Print_Values(struct Camera camera);

#endif // __CAMERA_H__