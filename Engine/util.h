#ifndef __UTIL2_H__
#define __UTIL2_H__

// C STD
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

// OPENGL
#include "glad/glad.h"

// CGLM
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"

// SDL2
#include "SDL2/SDL.h"

// MACROS
#define assert__(x) for (; !(x); assert(x))
#define assert_message(x, str)  \
    for (; !(x); assert(x))     \
    {                           \
        fprintf(stderr, (str)); \
    }

#endif // __UTIL2_H__