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
#define _colour_BLUE E_THINGY "[34m"
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

//#define check_that(A, M, ...)        \
//    if (!(A))                        \
//    {                                \
//        log_error(M, ##__VA_ARGS__); \
//    }

#define check_that(EXPRESSION, ERROR_MESSAGE, ...) \
    ((void)((EXPRESSION) || log_error(ERROR_MESSAGE, ##__VA_ARGS__)))

//#define EXIT_ON_GL_ERROR
#define CHECK_GL_ERRORS_TOGGLE
// gl_function_name   - the name of the OpengGL function being checked
// function_called_in - the name of the function the OpenGL function is being used in
static void __gl_check_for_an_error(
    const char *gl_function_name, const char *function_called_in, const char *file_name, int line_number)
{
    GLenum error    = 0;
    bool   is_error = false;
    char  *enum_string;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        is_error = true;
        switch (error)
        {
        case GL_INVALID_ENUM:
            enum_string = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            enum_string = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            enum_string = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            enum_string = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            enum_string = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            enum_string = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            enum_string = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_CONTEXT_LOST:
            enum_string = "GL_CONTEXT_LOST";
            break;
        default:
            fprintf(stderr, "Unknown error code %d", error);
        }
        fprintf(stderr, _colour_RED "ERROR with : " _colour_BLUE "%s" _colour_COLOUR_X " - %s\n", gl_function_name, enum_string);
        fprintf(stderr, "(%s:%d - %s)\n", file_name, line_number, function_called_in);
    }
#ifdef EXIT_ON_GL_ERROR
    if (is_error)
    {
        exit(1);
    }
#endif
}

#ifdef CHECK_GL_ERRORS_TOGGLE
#define CHECK_GL_ERRORS __gl_check_for_an_error("An OpenGL function call has a problem...", __FUNCTION__, __FILE__, __LINE__)
#define CHECK_GL_FUNC(FUNCTION) \
    (FUNCTION);                 \
    __gl_check_for_an_error(#FUNCTION, __FUNCTION__, __FILE__, __LINE__);
#else
#define CHECK_GL_ERRORS
#define CHECK_GL_FUNC
#endif

static void GLAPIENTRY MessageCallback(const GLenum  source,
                                       const GLenum  type,
                                       const GLuint  id,
                                       const GLenum  severity,
                                       const GLsizei length,
                                       const GLchar *message,
                                       const void   *userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        // fprintf(stderr, _colour_RED "[GL CALLBACK] Severity: High (id %d)" _colour_COLOUR_X, id);
        fprintf(stderr, _colour_RED "[GL CALLBACK] Severity: High" _colour_COLOUR_X);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        fprintf(stderr, _colour_YELLOW "[GL CALLBACK] Severity: Medium" _colour_COLOUR_X);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        fprintf(stderr, "[GL CALLBACK] Severity: Low ");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        fprintf(stderr, "[GL CALLBACK] Severity: Notification \n");
        // break;
        return;
    }

    printf(" - ");

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf("Other");
        break;
    }

    printf(" - ");

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Other");
        break;
    }

    // fprintf(stderr, "\n");
    fprintf(stderr, "\n%s\n", message);
}

#endif // __UTIL2_H__