#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "util.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

VAO_t Cube_Generate();
VAO_t Quad_Generate();
VAO_t Sphere_Generate();

inline void Cube_Draw(const VAO_t cube_vao)
{
    // Draw Cube
    VAO_Bind(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

inline void Quad_Draw(const VAO_t quad_vao)
{
    // Draw Quad
    VAO_Bind(quad_vao);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
}

inline void Sphere_Draw(const VAO_t sphere_vao)
{
    // Draw Sphere
    VAO_Bind(sphere_vao);
    glDrawElements(GL_TRIANGLE_STRIP, 8320, GL_UNSIGNED_INT, 0);
}

#endif // __SHAPES_H__