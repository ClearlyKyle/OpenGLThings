#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "..\BasicTriangle.h"

typedef void (*Windowfunction_ptr)();

struct Window
{
    SDL_Window *sdl_window;
    SDL_GLContext sdl_gl_context;

    int window_width;
    int window_heigh;

    Windowfunction_ptr Init;
    Windowfunction_ptr Update;

    bool quit;
};

void Window_Init(int width, int height, Windowfunction_ptr init, Windowfunction_ptr update);
void Window_Loop();

#endif // __WINDOW_H__