#include "Bricks.h"

Bricks_t Bricks_Init()
{
    Bricks_t bricks;

    bricks.width  = 51.0f;
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

    bricks.count = 6;

    VAO_t bricks_VAO = VAO_Create();

    VBO_t bricks_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(bricks_VBO, sizeof(bricks_vertices), (const GLvoid *)bricks_vertices);

    VAO_Attr(bricks_VAO, bricks_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    bricks.vao = bricks_VAO;

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            const int   index = row * 6 + col;
            const float x     = (4.0f + bricks.width) * (1 + col) + bricks.width * col;
            const float y     = (float)window.heigh - ((4.0f + bricks.height) * (1 + row) + bricks.height * row);

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

    for (int i = 0; i < 36; i++)
    {
        if (bricks->isHit[i])
            continue;

        Shader_Uniform_Mat4(shader, "model", bricks->model[i]);
        glDrawArrays(GL_TRIANGLES, 0, bricks->count);
    }
}

void Bricks_Destroy(Bricks_t *const bricks)
{
    VAO_Destroy(bricks->vao);
}
