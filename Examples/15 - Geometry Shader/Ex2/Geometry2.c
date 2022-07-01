#include "Geometry2.h"

static struct Geo
{
    VAO_t    vao;
    Shader_t shader;
} geo;

void Geometry2_Init()
{
    Shader_t shader = Shader_Create2("../../Examples/15 - Geometry Shader/Ex2/default.vs",
                                     "../../Examples/15 - Geometry Shader/Ex2/default.fs",
                                     "../../Examples/15 - Geometry Shader/Ex2/default.gs",
                                     3,
                                     (struct VertexAttribute[]){
                                         {.index = 0, .name = "aPos"},
                                         {.index = 1, .name = "aColor"},
                                         {.index = 2, .name = "aSides"},
                                     });

    const GLfloat points[] = {
        //  Coordinates     Color             Sides
        -0.45f, 0.45f, 1.0f, 0.0f, 0.0f, 4.0f,
        0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 8.0f,
        0.45f, -0.45f, 0.0f, 0.0f, 1.0f, 16.0f,
        -0.45f, -0.45f, 1.0f, 1.0f, 0.0f, 32.0f};

    VAO_t vao = VAO_Create();
    VBO_t vbo = VBO_Create(GL_ARRAY_BUFFER);

    VBO_Buffer(vbo, sizeof(points), (const GLvoid *)(points));

    VAO_Attr(vao, vbo, 0, 2, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));
    VAO_Attr(vao, vbo, 2, 1, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(5 * sizeof(GLfloat)));

    geo.vao    = vao;
    geo.shader = shader;
}

void Geometry2_Update()
{
    Shader_Bind(geo.shader);
    VAO_Bind(geo.vao);
    glDrawArrays(GL_POINTS, 0, 4);
}

void Geometry2_OnExit()
{
    VAO_Destroy(geo.vao);
    Shader_Destroy(&geo.shader);
}
