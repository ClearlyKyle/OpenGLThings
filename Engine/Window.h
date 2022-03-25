#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "util.h"

#include "SDL2/SDL.h"

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