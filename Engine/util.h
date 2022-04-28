#ifndef __UTIL2_H__
#define __UTIL2_H__

// C STD
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// OPENGL
#include "glad/glad.h"

// CGLM
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"

// SDL2
#include "SDL2/SDL.h"

// MACROS
#if _MSC_VER
#define E_THINGY "\33"
#else
#define E_THINGY "\e
#endif

#define _colour_RED E_THINGY "[31m"
#define _colour_GREEN E_THINGY "[32m"
#define _colour_YELLOW E_THINGY "[33m"
#define _colour_WHITE E_THINGY "[1m"
#define _colour_COLOUR_X E_THINGY "[m" // Resets the colour

// Need to add '\n' to the end of the message you want to log
#define __clean_errno() (errno == 0 ? "None" : strerror(errno))

// TODO : Toggle messaging levels (error only, error + warn, all)
#define log_error(M, ...) fprintf(stderr, _colour_RED "= ERROR =" _colour_COLOUR_X " (%s:%d: %s) " M, __FILE__, __LINE__, __clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, _colour_YELLOW "= WARN ==" _colour_COLOUR_X " (%s:%d:%s: %s) " M, __FILE__, __LINE__, __func__, __clean_errno(), ##__VA_ARGS__)
#define log_msg(M, ...) fprintf(stderr, _colour_WHITE "= LOG === " _colour_COLOUR_X M "\n", ##__VA_ARGS__)

//#define log_error(M, ...) fprintf(stderr, RED "[ERROR]" COLOUR_X "(%s:%d: %s) " M, __FILE__, __LINE__, __clean_errno(), __VA_OPT__(, ) __VA_ARGS__)
//#define log_warn(M, ...) fprintf(stderr, YELLOW "[WARN]" COLOUR_X " (%s:%d:%sЖ %s) " M, __FILE__, __LINE__, __func__, __clean_errno(), __VA_OPT__(, ) __VA_ARGS__)
//#define log_msg(M, ...) fprintf(stderr, WHITE "[LOG]" COLOUR_X M "\n" __VA_OPT__(, ) __VA_ARGS__)

#define assert_msg(A, M, ...)        \
    if (!(A))                        \
    {                                \
        log_error(M, ##__VA_ARGS__); \
        assert(A);                   \
    }

#define check_that(A, M, ...)        \
    if (!(A))                        \
    {                                \
        log_error(M, ##__VA_ARGS__); \
    }

#define EXIT_ON_GL_ERROR
#define CHECK_GL_ERRORS_TOGGLE

static void gl_check_error(const char *function, const char *file, int line)
{
    GLenum error    = 0;
    bool   is_error = false;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        is_error = true;
        switch (error)
        {
        case GL_INVALID_ENUM:
            printf("GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE:
            printf("GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION:
            printf("INVALID_OPERATION");
            break;
        case GL_STACK_OVERFLOW:
            printf("STACK_OVERFLOW");
            break;
        case GL_STACK_UNDERFLOW:
            printf("STACK_UNDERFLOW");
            break;
        case GL_OUT_OF_MEMORY:
            printf("OUT_OF_MEMORY");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            printf("INVALID_FRAMEBUFFER_OPERATION");
            break;
        case GL_CONTEXT_LOST:
            printf("GL_CONTEXT_LOST");
            break;
        // case GL_TABLE_TOO_LARGE:
        //     printf("GL_TABLE_TOO_LARGE");
        //     break;
        default:
            printf("Unknown error code %d", error);
        }
        printf(" : '%s' (%s:%d)\n", function, file, line);
    }
#ifdef EXIT_ON_GL_ERROR
    if (is_error)
    {
        exit(1);
    }
#endif
}

#ifdef CHECK_GL_ERRORS_TOGGLE
#define CHECK_GL_ERRORS gl_check_error(__FUNCTION__, __FILE__, __LINE__)
#else
#define CHECK_GL_ERRORS
#endif

#endif // __UTIL2_H__