#include "Breakout.h"

#include "Ball.h"
#include "Paddle.h"
#include "Collisions.h"
#include "Bricks.h"

struct Breakout
{
    Shader_t shader;

    Ball_t   ball;
    Paddle_t paddle;
    Bricks_t bricks;
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
    Paddle_t paddle = Paddle_Init(20.0f, 50.0f, 7.0f);
    Bricks_t bricks = Bricks_Init();

    bo.ball   = ball;
    bo.paddle = paddle;
    bo.bricks = bricks;
    bo.shader = shader;
}

void Breakout_Update()
{
    Ball_Update(&bo.ball);
    Ball_Draw(bo.shader, &bo.ball);

    Paddle_Update(&bo.paddle);
    Paddle_Draw(bo.shader, &bo.paddle);

    Bricks_Draw(bo.shader, &bo.bricks);

    if (Ball_Paddle_Collision(bo.ball, bo.paddle))
    {
        bo.ball.dy *= -1.05f;
    }

    Ball_Brick_Collision(bo.ball, &bo.bricks);
}

void Breakout_OnExit()
{
    Shader_Destroy(&bo.shader);

    Ball_Destroy(&bo.ball);
    Paddle_Destroy(&bo.paddle);
    Bricks_Destroy(&bo.bricks);
}
