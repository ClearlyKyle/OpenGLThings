#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "util.h"
#include "Inputs.h"

// ALT + R (performace overlay with nvidia)

typedef void (*Windowfunction_ptr)();

struct Window
{
    SDL_Window   *sdl_window;
    SDL_GLContext sdl_gl_context;
    SDL_Event     event;

    unsigned int width;
    unsigned int heigh;
    float        aspect_ratio;

    Windowfunction_ptr Init;
    Windowfunction_ptr Update;
    Windowfunction_ptr OnExit;

    bool quit;

    struct InputManager input;

    double frame_time, last_frame_time; // ms
};

// global window
extern struct Window window;

void Window_Init(int width, int height, Windowfunction_ptr init, Windowfunction_ptr update, Windowfunction_ptr on_exit);
void Window_Clear_Colour(const float r, const float g, const float b, const float a);
void Window_Loop();

#endif // __WINDOW_H__