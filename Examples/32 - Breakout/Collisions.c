#include "Collisions.h"

#include <math.h>
#include "cglm/cglm.h"

static float clamp(float value, float min, float max)
{
    return (float)fmax(min, fmin(max, value));
}

static bool Circle_Rect_Collision(
    float circle_x, float circle_y, float radius,
    float rect_x, float rect_y, float width, float height)
{
    vec2 circle_position = {circle_x, circle_y};

    // calculate AABB info (center, half-extents)
    vec2 aabb_half_extents = {width / 2.0f, height / 2.0f};
    vec2 aabb_center       = {
        rect_x + aabb_half_extents[0],
        rect_y + aabb_half_extents[1]};

    // get difference vector between both centers
    vec2 difference;

    glm_vec2_sub(circle_position, aabb_center, difference);

    vec2 clamped;
    clamped[0] = clamp(difference[0], -aabb_half_extents[0], aabb_half_extents[0]);
    clamped[1] = clamp(difference[1], -aabb_half_extents[1], aabb_half_extents[1]);

    // add clamped value to AABB_center and we get the value of box closest to circle
    vec2 closest;
    glm_vec2_add(aabb_center, clamped, closest);

    // retrieve vector between center circle and closest point AABB and check if length <= radius
    glm_vec2_sub(closest, circle_position, difference);

    return glm_vec2_norm(difference) < radius;
}

bool Ball_Paddle_Collision(const Ball_t ball, const Paddle_t paddle)
{
    return Circle_Rect_Collision(
        ball.pos[0], ball.pos[1], ball.radius,
        paddle.pos[0], paddle.pos[1], paddle.width, paddle.height);
}

bool Ball_Brick_Collision(const Ball_t ball, Bricks_t *bricks)
{
    for (int i = 0; i < 36; i++)
    {
        if (Circle_Rect_Collision(
                ball.pos[0], ball.pos[1], ball.radius,
                bricks->pos[i][0], bricks->pos[i][1], bricks->width, bricks->height))
        {
            bricks->isHit[i] = true;
        }
    }

    return true;
}
