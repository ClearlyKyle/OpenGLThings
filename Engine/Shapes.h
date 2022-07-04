#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "util.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

VAO_t Cube_Generate();
VAO_t Quad_Generate();

inline void Cube_Draw(const VAO_t cube_vao)
{
    // Draw cube
    VAO_Bind(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

inline void Quad_Draw(const VAO_t quad_vao)
{
    // Draw Quad
    VAO_Bind(quad_vao);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

#endif // __SHAPES_H__