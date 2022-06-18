#include "Breakout.h"

#define NUM_SEGMENTS 100

struct Circle
{
    Shader_t shader;
    VAO_t    vao;
    GLsizei  count;

    float dx;
    float dy;

    vec3 pos;
} circle;

void Breakout_Init()
{
    GLfloat circle_vertices[6 * NUM_SEGMENTS] = {0};

    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        const double angle     = i * 2.0 * M_PI / (double)(NUM_SEGMENTS - 1);
        const double nextAngle = (i + 1) * 2.0 * M_PI / (double)(NUM_SEGMENTS - 1);

        // Vertex 1
        circle_vertices[i * 6 + 0] = (GLfloat)(cos(angle) * 20.0); // X
        circle_vertices[i * 6 + 1] = (GLfloat)(sin(angle) * 20.0); // Y

        // Vertex 2
        circle_vertices[i * 6 + 2] = (GLfloat)(cos(nextAngle) * 20.0); // X
        circle_vertices[i * 6 + 3] = (GLfloat)(sin(nextAngle) * 20.0); // Y

        // Vertex 3
        circle_vertices[i * 6 + 4] = 0.0f; // Origin
        circle_vertices[i * 6 + 5] = 0.0f;
    }

    circle.count = 6 * NUM_SEGMENTS;

    const Shader_t shader = Shader_Create("../../Examples/32 - Breakout/default.vs",
                                          "../../Examples/32 - Breakout/default.fs",
                                          1,
                                          (struct VertexAttribute[]){
                                              {.index = 0, .name = "aPos"},
                                          });

    VAO_t circle_VAO = VAO_Create();

    VBO_t circle_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(circle_VBO, sizeof(circle_vertices), (const GLvoid *)circle_vertices);

    VAO_Attr(circle_VAO, circle_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    // mat4 orthograph;
    //// mat4_orthagonal(WIDTH, HEIGHT, orthograph);
    // glm_ortho_default(window.aspect_ratio, orthograph);

    mat4 orthograph = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(0.0f, window.width, window.heigh, 0.0f, -1.0f, 1.0f, orthograph);

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
    // if (Input_Key_Down(window.input, SDL_SCANCODE_D))
    //{
    //     glm_vec3_muladd(speed, camera->orientation, camera->position);
    // }

    const double tm = window.frame_time * 10.0;

    circle.pos[0] += (circle.dx * (float)tm);
    circle.pos[1] += (circle.dy * (float)tm);

    if (circle.pos[0] > (float)window.width)
    {
        circle.pos[0] = (float)window.width;
        circle.dx *= -1;
    }
    else if (circle.pos[0] < 0)
    {
        circle.pos[0] = 0;
        circle.dx *= -1;
    }

    if (circle.pos[1] > (float)window.heigh)
    {
        circle.pos[1] = (float)window.heigh;
        circle.dy *= -1;
    }
    else if (circle.pos[1] < 0)
    {
        circle.pos[1] = 0;
        circle.dy *= -1;
    }

    mat4 model;
    glm_translate_make(model, circle.pos);

    Shader_Bind(circle.shader);
    Shader_Uniform_Mat4(circle.shader, "model", model);
    Shader_Uniform_Vec3(circle.shader, "pos", circle.pos);

    VAO_Bind(circle.vao);
    glDrawArrays(GL_TRIANGLES, 0, circle.count);
}

void Breakout_OnExit()
{
    VAO_Destroy(circle.vao);
    Shader_Destroy(&circle.shader);
}
