#include "Ball.h"

#define NUM_SEGMENTS 100

Ball_t Ball_Init(const float x, const float y, const float radius)
{
    Ball_t ball;

    GLfloat ball_vertices[6 * NUM_SEGMENTS] = {0};

    ball.count = 6 * NUM_SEGMENTS;

    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        const float angle     = i * 2.0f * (float)M_PI / (float)(NUM_SEGMENTS - 1);
        const float nextAngle = (i + 1) * 2.0f * (float)M_PI / (float)(NUM_SEGMENTS - 1);

        // Vertex 1
        ball_vertices[i * 6 + 0] = (GLfloat)(cosf(angle) * radius); // X
        ball_vertices[i * 6 + 1] = (GLfloat)(sinf(angle) * radius); // Y

        // Vertex 2
        ball_vertices[i * 6 + 2] = (GLfloat)(cosf(nextAngle) * radius); // X
        ball_vertices[i * 6 + 3] = (GLfloat)(sinf(nextAngle) * radius); // Y

        // Vertex 3
        ball_vertices[i * 6 + 4] = 0.0f; // Origin
        ball_vertices[i * 6 + 5] = 0.0f;
    }

    VAO_t ball_VAO = VAO_Create();

    VBO_t ball_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(ball_VBO, sizeof(ball_vertices), (const GLvoid *)ball_vertices);

    VAO_Attr(ball_VAO, ball_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    glm_vec3_copy((vec3){x, y, 0.0f}, ball.pos);

    ball.vao = ball_VAO;

    ball.dx = 2.0f;
    ball.dy = 3.0f;

    ball.radius = radius;

    return ball;
}

void Ball_Update(Ball_t *const ball)
{
    const double tm = window.frame_time * 100.0;

    ball->pos[0] += (ball->dx * (float)tm);
    ball->pos[1] += (ball->dy * (float)tm);

    if (ball->pos[0] > (float)window.width)
    {
        ball->pos[0] = (float)window.width;
        ball->dx *= -1;
    }
    else if (ball->pos[0] < 0)
    {
        ball->pos[0] = 0;
        ball->dx *= -1;
    }

    if (ball->pos[1] > (float)window.heigh)
    {
        ball->pos[1] = (float)window.heigh;
        ball->dy *= -1;
    }
    else if (ball->pos[1] < 0)
    {
        ball->pos[1] = 0;
        ball->dy *= -1;
    }

    glm_translate_make(ball->model, ball->pos);
}

void Ball_Draw(const Shader_t shader, const Ball_t *const ball)
{
    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "model", ball->model);
    Shader_Uniform_Vec3(shader, "pos", ball->pos);

    VAO_Bind(ball->vao);
    glDrawArrays(GL_TRIANGLES, 0, ball->count);
}

void Ball_Destroy(Ball_t *const ball)
{
    VAO_Destroy(ball->vao);
}