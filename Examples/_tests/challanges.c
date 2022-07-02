
#include "challanges.h"

struct challanges
{
    struct VAO    vao;
    struct Shader shader;

    GLsizei size;
} challange;

void challanges_Init()
{
    GLfloat verts[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};

    GLuint indicies[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        0, 2};

    challange.size = sizeof(indicies) / sizeof(GLuint);

    struct Shader shader = Shader_Create(
        "../../Examples/_tests/default.vs",
        "../../Examples/_tests/default.fs",
        1,
        (struct VertexAttribute[]){{.index = 0, .name = "aPos"}});

    challange.shader = shader;

    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    challange.vao = vao;

    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(verts), (const GLvoid *)verts);

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indicies), (const GLvoid *)indicies);

    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    // VAO_Unbind();
    // VBO_Unbind();
    // EBO_Unbind();
}

void challanges_Update()
{
    Shader_Bind(challange.shader);
    VAO_Bind(challange.vao);

    // glDrawArrays(GL_TRIANGLES, 0, challange.size);

    // glDrawElements(GL_TRIANGLES, challange.size, GL_UNSIGNED_INT, 0);

    // GL_LINES  - draws a single line from 2 indicies, indicies need to be paired.
    glDrawElements(GL_LINES, challange.size, GL_UNSIGNED_INT, 0);
}

void challanges_OnExit()
{
    Shader_Destroy(&challange.shader);
    VBO_Unbind();
    VAO_Unbind();
}
