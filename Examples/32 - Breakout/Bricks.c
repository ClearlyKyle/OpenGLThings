#include "Bricks.h"

Bricks_t Bricks_Init()
{
    Bricks_t bricks;

    const float column_spacing = 10.0f;

    // width from left to right side
    const float brick_width = ((float)window.width - column_spacing) / ((float)NUM_COLUMNS) - column_spacing;
    // since we are drawing rectangles with the center in the middle of rectangle
    // the width is actually half the over all widthd

    bricks.width  = brick_width / 2.0f; // half width as the vertcies coordinates are centered on the origin
    bricks.height = 13.0f;

    // To initialize all the elements to false
    memset(bricks.isHit, 0, sizeof(bricks.isHit));

    const GLfloat bricks_vertices[] = {
        -bricks.width, -bricks.height,
        -bricks.width, bricks.height,
        bricks.width, bricks.height,
        bricks.width, bricks.height,
        bricks.width, -bricks.height,
        -bricks.width, -bricks.height};

    bricks.vertex_count = 6;

    VAO_t bricks_VAO = VAO_Create();

    VBO_t bricks_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(bricks_VBO, sizeof(bricks_vertices), (const GLvoid *)bricks_vertices);

    VAO_Attr(bricks_VAO, bricks_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    bricks.vao = bricks_VAO;

    for (int row = 0; row < NUM_ROW; row++)
    {
        for (int col = 0; col < NUM_COLUMNS; col++)
        {
            const int index = row * NUM_COLUMNS + col;

            // first we need to jump along a distance of: (4.0f + bricks.width) * (1 + col)
            // then add on the remaining part of the brick: bricks.width * col
            /*   __________
                |   _____
                |  |_____|
                ^-----^
                  D1  ^--^
                       D2
             */

            const float x = (column_spacing + bricks.width) * (1 + col) + bricks.width * col;
            const float y = (float)window.heigh - ((4.0f + bricks.height) * (1 + row) + bricks.height * row);

            bricks.pos[index][0] = (float)x;
            bricks.pos[index][1] = (float)y;
            bricks.pos[index][2] = 0.0f;

            glm_translate_make(bricks.model[index], bricks.pos[index]);
        }
    }

    return bricks;
}

void Bricks_Draw(const Shader_t shader, const Bricks_t *const bricks)
{
    Shader_Bind(shader);
    VAO_Bind(bricks->vao);

    for (int i = 0; i < NUM_COLUMNS * NUM_ROW; i++)
    {
        if (bricks->isHit[i])
            continue;

        Shader_Uniform_Mat4(shader, "model", bricks->model[i]);
        glDrawArrays(GL_TRIANGLES, 0, bricks->vertex_count);
    }
}

void Bricks_Destroy(Bricks_t *const bricks)
{
    VAO_Destroy(bricks->vao);
}
