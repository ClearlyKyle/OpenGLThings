#include "Window.h"

// global window
struct Window window;

void Window_Init(int width, int height)
{
    window.window_width = width;
    window.window_heigh = height;

    // initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL cannot init with error: %s\n", SDL_GetError());
        exit(2);
    }
    // create window
    window.sdl_window = SDL_CreateWindow("Opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window.sdl_window == NULL)
    {
        fprintf(stderr, "[SDL_CreateWindow] Cannot create window with error: %s\n", SDL_GetError());
        exit(2);
    }
    // create opengl context
    window.sdl_gl_context = SDL_GL_CreateContext(window.sdl_window);
    if (window.sdl_gl_context == NULL)
    {
        fprintf(stderr, "[SDL_GL_CreateContext] Cannot create OpenGL context with error: %s\n", SDL_GetError());
        exit(2);
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "[gladLoadGLLoader] Failed to initialize OpenGL context: %s\n", SDL_GetError());
        exit(2);
    }

    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Set to 1 to require hardware acceleration, set to 0 to force software rendering
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Use v-sync: (0) - off, (1) - on
    SDL_GL_SetSwapInterval(0);

    glViewport(0, 0, window.window_width, window.window_heigh);

    // Set Clear Colour
    glClearColor(0.18431f, 0.31765f, 0.71373f, 1.0f); // r g b a
}

static void Window_Destroy()
{
    SDL_DestroyWindow(window.sdl_window);
    window.sdl_window = NULL;
    SDL_Quit();
}

void Window_Loop()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {
            -0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,  // Lower left corner
            0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,   // Lower right corner
            0.0f, 0.5f * (float)sqrt(3) * 2 / 3, 0.0f // Upper corner
        };

    SDL_Event event;
    while (!window.quit)
    {
        Uint64 start = SDL_GetPerformanceCounter();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                window.quit = true;
            }
        }

        

        SDL_GL_SwapWindow(window.sdl_window);

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
        printf("Current FPS: %0.3f\n", 1.0f / elapsed);
    }

    Window_Destroy();
}