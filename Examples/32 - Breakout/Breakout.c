#include "Breakout.h"

#define NUM_SEGMENTS 100

struct Circle
{
    Shader_t shader;
    VAO_t    vao;

    float dx;
    float dy;

    vec3 pos;
} circle;

void Breakout_Init()
{
    GLfloat circle_vertices[6 * NUM_SEGMENTS];

    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        const float angle     = i * 2.0f * M_PI / (NUM_SEGMENTS - 1);
        const float nextAngle = (i + 1) * 2.0f * M_PI / (NUM_SEGMENTS - 1);

        // Vertex 1
        circle_vertices[i * 6 + 0] = cos(angle) * 20; // X
        circle_vertices[i * 6 + 1] = sin(angle) * 20; // Y

        // Vertex 2
        circle_vertices[i * 6 + 2] = cos(nextAngle) * 20; // X
        circle_vertices[i * 6 + 3] = sin(nextAngle) * 20; // Y

        // Vertex 3
        circle_vertices[i * 6 + 4] = 0.0f; // Origin
        circle_vertices[i * 6 + 5] = 0.0f;
    }

    const Shader_t shader = Shader_Create("../../Examples/32 - Breakout/default.vs",
                                          "../../Examples/32 - Breakout/default.fs",
                                          1,
                                          (struct VertexAttribute[]){
                                              {.index = 0, .name = "aPos"},
                                          });

    VAO_t circle_VAO = VAO_Create();
    VBO_t circle_VBO = VBO_Create(GL_ARRAY_BUFFER);

    VBO_Buffer(circle_VBO, sizeof(circle_vertices), (const GLvoid *)circle_vertices);

    VAO_Attr(circle_VAO, circle_VBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)0);

    mat4 orthograph;
    // mat4_orthagonal(WIDTH, HEIGHT, orthograph);
    glm_ortho_default(window.aspect_ratio, orthograph);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "orthograph", orthograph);

    vec3 pos = {50.0f, 50.0f, 0.0f};

    glm_vec3_copy(pos, circle.pos);

    circle.shader = shader;
    circle.vao    = circle_VAO;

    circle.dx = 2.0f;
    circle.dy = 3.0f;
}

void Breakout_Update()
{
    circle.pos[0] += circle.dx;
    circle.pos[1] += circle.dy;

    if (circle.pos[0] > window.width)
    {
        circle.pos[0] = window.width;
        circle.dx *= -1;
    }
    else if (circle.pos[0] < 0)
    {
        circle.pos[0] = 0;
        circle.dx *= -1;
    }

    if (circle.pos[1] > window.heigh)
    {
        circle.pos[1] = window.heigh;
        circle.dy *= -1;
    }
    else if (circle.pos[1] < 0)
    {
        circle.pos[1] = 0;
        circle.dy *= -1;
    }

    mat4 mvp;
    glm_translate_make(mvp, circle.pos);

    Shader_Bind(circle.shader);
    Shader_Uniform_Mat4(circle.shader, "", mvp);

    VAO_Bind(circle.vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * NUM_SEGMENTS);
}

void Breakout_OnExit()
{
    VAO_Destroy(circle.vao);
    Shader_Destroy(&circle.shader);
}
