#include "Paddle.h"

Paddle_t Paddle_Init(const float y, const float width, const float height)
{
    Paddle_t paddle;
    paddle.width  = width;
    paddle.height = height;

    const GLfloat paddle_vertices[] = {
        -paddle.width, -paddle.height,
        -paddle.width, paddle.height,
        paddle.width, paddle.height,
        paddle.width, paddle.height,
        paddle.width, -paddle.height,
        -paddle.width, -paddle.height};

    VAO_t paddle_VAO = VAO_Create();

    VBO_t paddle_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(paddle_VBO, sizeof(paddle_vertices), (const GLvoid *)paddle_vertices);

    VAO_Attr(paddle_VAO, paddle_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    glm_vec3_copy((vec3){(float)window.width / 2.0f, y, 0.0f}, paddle.pos);

    paddle.count = 6;
    paddle.vao   = paddle_VAO;
    paddle.dx    = 2.0f;
    paddle.dy    = 0.0f;

    return paddle;
}

void Paddle_Update(Paddle_t *const paddle)
{
    const double tm = window.frame_time * 500.0;

    if (Input_Key_Down(window.input, SDL_SCANCODE_A) || Input_Key_Down(window.input, SDL_SCANCODE_LEFT))
    {
        paddle->pos[0] -= paddle->dx * (float)tm;
    }

    if (Input_Key_Down(window.input, SDL_SCANCODE_D) || Input_Key_Down(window.input, SDL_SCANCODE_RIGHT))
    {
        paddle->pos[0] += paddle->dx * (float)tm;
    }

    if (paddle->pos[0] < 0)
    {
        paddle->pos[0] = 0.0f;
    }
    else if (paddle->pos[0] > (float)window.width)
    {
        paddle->pos[0] = (float)window.width;
    }

    glm_translate_make(paddle->model, paddle->pos);
}

void Paddle_Draw(const Shader_t shader, const Paddle_t *const paddle)
{
    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "model", paddle->model);
    Shader_Uniform_Vec3(shader, "pos", paddle->pos);

    VAO_Bind(paddle->vao);
    glDrawArrays(GL_TRIANGLES, 0, paddle->count);
}

void Paddle_Destroy(Paddle_t *const paddle)
{
    VAO_Destroy(paddle->vao);
}
