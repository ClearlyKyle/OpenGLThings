#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <stdbool.h>

#include "Ball.h"
#include "Paddle.h"

// Ball_t, Paddle_t collision
bool Ball_Paddle_Collision(const Ball_t ball, const Paddle_t paddle);

#endif // __COLLISIONS_H__