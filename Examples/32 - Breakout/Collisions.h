#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <stdbool.h>

#include "Ball.h"
#include "Paddle.h"
#include "Bricks.h"

// Ball_t, Paddle_t collision
bool Ball_Paddle_Collision(const Ball_t ball, const Paddle_t paddle);
bool Ball_Brick_Collision(const Ball_t ball, Bricks_t *bricks);

#endif // __COLLISIONS_H__