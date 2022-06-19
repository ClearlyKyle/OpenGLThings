#include "Paddle.h"

Paddle_t Paddle_Init(const float y)
{
    const GLfloat paddle_vertices[] = {
        -50.0f, -7.0f,
        -50.0f, 7.0f,
        50.0f, 7.0f,
        50.0f, 7.0f,
        50.0f, -7.0f,
        -50.0f, -7.0f};

    VAO_t paddle_VAO = VAO_Create();

    VBO_t paddle_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(paddle_VBO, sizeof(paddle_vertices), (const GLvoid *)paddle_vertices);

    VAO_Attr(paddle_VAO, paddle_VBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)0);

    Paddle_t paddle;

    glm_vec3_copy((vec3){(float)window.width / 2.0f, y, 0.0f}, paddle.pos);

    paddle.count = 12;
    paddle.vao   = paddle_VAO;
    paddle.dx    = 2.0f;
    paddle.dy    = 0.0f;

    return paddle;
}

void Paddle_Update(Paddle_t *const paddle)
{
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
