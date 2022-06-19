#include "Breakout.h"

#include "Ball.h"
#include "Paddle.h"

struct Breakout
{
    Shader_t shader;

    Ball_t   ball;
    Paddle_t paddle;

} bo;

void Breakout_Init()
{
    const Shader_t shader = Shader_Create("../../Examples/32 - Breakout/default.vs",
                                          "../../Examples/32 - Breakout/default.fs",
                                          1,
                                          (struct VertexAttribute[]){
                                              {.index = 0, .name = "aPos"},
                                          });

    mat4 orthograph = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(0.0f, (float)window.width, 0.0f, (float)window.heigh, -1.0f, 1.0f, orthograph);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "orthograph", orthograph);

    Ball_t   ball   = Ball_Init(50.0f, 50.0f, 20.0f);
    Paddle_t paddle = Paddle_Init(20.0f);

    bo.ball   = ball;
    bo.paddle = paddle;
    bo.shader = shader;
}

void Breakout_Update()
{
    // if (Input_Key_Down(window.input, SDL_SCANCODE_D))
    //{
    //     glm_vec3_muladd(speed, camera->orientation, camera->position);
    // }

    Ball_Update(&bo.ball);
    Ball_Draw(bo.shader, &bo.ball);

    Paddle_Update(&bo.paddle);
    Paddle_Draw(bo.shader, &bo.paddle);
}

void Breakout_OnExit()
{
    Shader_Destroy(&bo.shader);

    Ball_Destroy(&bo.ball);
    Paddle_Destroy(&bo.paddle);
}
