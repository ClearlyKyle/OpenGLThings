#include "Collisions.h"

#include <math.h>
#include "cglm/cglm.h"

static float clamp(float value, float min, float max)
{
    return fmax(min, fmin(max, value));
}

bool Ball_Paddle_Collision(const Ball_t ball, const Paddle_t paddle)
{
    // vec2 center;

    //// get center point circle first
    // glm_vec2_add(ball.pos, paddle.pos, center);

    //// calculate AABB info (center, half-extents)
    // vec2 aabb_half_extents = {paddle.width / 2.0f, paddle.height / 2.0f};
    // vec2 aabb_center       = {
    //     ball.pos[0] + aabb_half_extents[0],
    //     ball.pos[1] + aabb_half_extents[1]};

    // get difference vector between both centers
    // vec2 difference;

    // glm_vec2_sub(center, aabb_center, difference);
    // glm_vec2_clamp(difference, )
    // glm::vec2 difference = center - aabb_center;
    // glm::vec2 clamped    = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    //// add clamped value to AABB_center and we get the value of box closest to circle
    // glm::vec2 closest = aabb_center + clamped;

    //// retrieve vector between center circle and closest point AABB and check if length <= radius
    // difference = closest - center;
    // return glm::length(difference) < one.Radius;

    // Find the closest point to the circle within the rectangle
    const float closestX = clamp(ball.pos[0], paddle.pos[0], paddle.pos[0] + paddle.width);
    const float closestY = clamp(ball.pos[1], paddle.pos[1] + paddle.height, paddle.pos[1]);

    // Calculate the distance between the circle's center and this closest point
    const float distanceX = ball.pos[0] - closestX;
    const float distanceY = ball.pos[1] - closestY;

    // If the distance is less than the circle's radius, an intersection occurs
    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    return distanceSquared < (ball.radius * ball.radius);
}
