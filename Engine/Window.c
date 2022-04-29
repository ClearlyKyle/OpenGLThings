#include "Window.h"

struct Window window;

void Window_Init(int width, int height,
                 Windowfunction_ptr init,
                 Windowfunction_ptr update,
                 Windowfunction_ptr on_exit)
{
    window.width        = width;
    window.heigh        = height;
    window.Init         = init;
    window.Update       = update;
    window.OnExit       = on_exit;
    window.aspect_ratio = (float)width / (float)height;
    // window.sdl_event = event;

    // initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL cannot init with error: %s\n", SDL_GetError());
        exit(2);
    }

    // SETTING ATTRIBUTES
    //      SDL_GL_SetAttribute - needs to be used before "SDL_CreateWindow"

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // Set to 1 to require hardware acceleration, set to 0 to force software rendering

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Depth buffer size in bits
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // Stencil buffer size in bits

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

    // Check OpenGL properties
    printf("OpenGL loaded\n");
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    printf("\n");

    GLint stencil_bits, depth_bits;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    // SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_bits);
    // SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_bits);

    printf("Stencil Buffer Size  : %d (bits)\n", stencil_bits);
    printf("Depth Buffer Size    : %d (bits)\n", depth_bits);
    printf("\n");

    // ENABLE
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    // glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
    //                      GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here");
    //  glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    //  glDepthFunc(GL_LESS);

    // glEnable(GL_STENCIL_TEST); // Enable stencil test

    // Use v-sync: (0) - off, (1) - on
    SDL_GL_SetSwapInterval(0);

    glViewport(0, 0, window.width, window.heigh);

    // Set Clear Colour
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // r g b a
}

static void Window_Destroy()
{
    InputManager_Destroy(window.input);

    SDL_DestroyWindow(window.sdl_window);
    window.sdl_window = NULL;
    SDL_Quit();
}

static void _Init()
{
    window.Init();
}

static void _Update()
{
    window.Update();
}

static void _OnExit()
{
    window.OnExit();
}

void Window_Loop()
{
    // Init Inputs
    struct InputManager in = InputManager_Create();
    window.input           = in;

    _Init();

    float elapsed = 0.0f;

    // SDL_Init(SDL_INIT_EVENTS);
    // SDL_SetHint(SDL_HINT_EVENT_LOGGING, "2");

    bool first_click = true;

    while (!window.quit)
    {
        Uint64 start           = SDL_GetPerformanceCounter();
        window.last_frame_time = window.frame_time;
        window.frame_time      = elapsed;

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        Input_Update(&window.input);
        _Update();

        SDL_GL_SwapWindow(window.sdl_window);

        Uint64 end = SDL_GetPerformanceCounter();
        elapsed    = (end - start) / (float)SDL_GetPerformanceFrequency();
        // printf("Elapsed time : %0.3fms\tFPS : %0.3f\n", elapsed * 1000.0, 1.0f / elapsed);
    }

    _OnExit();
    Window_Destroy();
}