#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "glad/glad.h"

struct Window
{
    SDL_Window *sdl_window;
    SDL_GLContext sdl_gl_context;

    int window_width;
    int window_heigh;

    bool quit;
};

void Window_Init(int width, int height);
void Window_Loop();

#endif // __WINDOW_H__