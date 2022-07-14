#include "TextRendering.h"

#include "SDL2/SDL_ttf.h"

static struct TextRendering
{
    Camera_t cam;

    VAO_t     text_vao;
    Shader_t  text_shader;
    GLuint    texture;
    TTF_Font *font;

    Shader_t cube_shader;
    VAO_t    cube;
} tr;

static void Render_text(const ivec2 position, const char *message)
{
    Shader_Bind(tr.text_shader);

    SDL_Surface *surface_message = TTF_RenderText_Blended(tr.font, message, (SDL_Color){255, 255, 255});
    if (!surface_message)
    {
        fprintf(stderr, "Error using TTF_RenderText_Blended: %s\n", TTF_GetError());
        window.quit = true;
        return;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_message->w, surface_message->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface_message->pixels);

    const GLuint verts[] = {
        0, 0, position[0], position[1],                                           //  1,0 --- 1,1
        1, 0, position[0] + surface_message->w, position[1],                      //   |       |
        1, 1, position[0] + surface_message->w, position[1] + surface_message->h, //   |       |
        0, 1, position[0], position[1] + surface_message->h,                      //  0,0 --- 0,1
    };

    const GLuint indicies[] = {
        0, 3, 2, //
        0, 2, 1, //
    };

    VAO_t vao = VAO_Create();
    VBO_t vbo = VBO_Create(GL_ARRAY_BUFFER);

    EBO_t ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indicies), (const GLvoid *)indicies);

    VBO_Buffer(vbo, sizeof(verts), (const GLvoid *)verts);
    VAO_Attr(vao, vbo, 0, 4, GL_UNSIGNED_INT, 4 * sizeof(GLuint), (const GLvoid *)(0)); // TEX + POSITION (tex.x, tex.y, pos.x, pos.y)

    SDL_FreeSurface(surface_message);

    VAO_Bind(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TextRendering_Init()
{
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Camera, dont forget this!!
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    tr.cam       = cam;

    // Text position
    ivec2 position    = {10, 10};
    vec3  text_colour = {1.0f, 0.0f, 0.0f};

    mat4 projection;
    glm_ortho(0.0f, (float)window.width, (float)window.heigh, 0.0f, -1.0f, 1.0f, projection);

    // Shader
    Shader_t shader_text = Shader_Create("../../Examples/33 - Text Rendering/text2D.vs",
                                         "../../Examples/33 - Text Rendering/text2D.fs",
                                         1,
                                         (struct VertexAttribute[]){
                                             {.index = 0, .name = "vertex"},
                                         });
    Shader_t shader_cube = Shader_Create("../../Examples/33 - Text Rendering/cube.vs",
                                         "../../Examples/33 - Text Rendering/cube.fs",
                                         3,
                                         (struct VertexAttribute[]){
                                             {.index = 0, .name = "position"},
                                             {.index = 1, .name = "normal"},
                                             {.index = 2, .name = "texCoord"},
                                         });

    Shader_Bind(shader_text);
    Shader_Uniform_Int(shader_text, "textTexture", 0);
    Shader_Uniform_Mat4(shader_text, "projection", projection);
    Shader_Uniform_Vec3(shader_text, "textColour", text_colour);

    tr.text_shader = shader_text;
    tr.cube_shader = shader_cube;

    // Initialise TTF
    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        window.quit = true;
        return;
    }

    // Open the font
    const char *font_file = "../../Examples/res/fonts/ShipporiAntique-Regular.ttf";
    int         font_size = 16;

    TTF_Font *font = TTF_OpenFont(font_file, font_size);
    if (font == NULL)
    {
        fprintf(stderr, "Error using TTF_OpenFont: %s\nFont Path: %s", TTF_GetError(), font_file);
        window.quit = true;
        return;
    }
    tr.font = font;

    // TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    const char  *message         = "Hello WORLD!";
    SDL_Surface *surface_message = TTF_RenderText_Blended(font, message, (SDL_Color){255, 255, 255});
    if (!surface_message)
    {
        fprintf(stderr, "Error using TTF_RenderText_Blended: %s\n", TTF_GetError());
        window.quit = true;
        return;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_message->w, surface_message->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface_message->pixels);

    tr.texture = texture;

    const GLuint verts[] = {
        0, 0, position[0], position[1],                                           //  1,0 --- 1,1
        1, 0, position[0] + surface_message->w, position[1],                      //   |       |
        1, 1, position[0] + surface_message->w, position[1] + surface_message->h, //   |       |
        0, 1, position[0], position[1] + surface_message->h,                      //  0,0 --- 0,1
    };

    const GLuint indicies[] = {
        0, 3, 2, //
        0, 2, 1, //
    };

    VAO_t vao = VAO_Create();
    VBO_t vbo = VBO_Create(GL_ARRAY_BUFFER);

    EBO_t ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indicies), (const GLvoid *)indicies);

    VBO_Buffer(vbo, sizeof(verts), (const GLvoid *)verts);
    VAO_Attr(vao, vbo, 0, 4, GL_UNSIGNED_INT, 4 * sizeof(GLuint), (const GLvoid *)(0)); // TEX + POSITION (tex.x, tex.y, pos.x, pos.y)

    tr.text_vao = vao;

    SDL_FreeSurface(surface_message);

    // Cube
    tr.cube = Cube_Generate();
}

void TextRendering_Update()
{
    Camera_Inputs(&tr.cam);

    // Showing frame time
    char buff[32];
    sprintf(buff, "FRAME TIME: %f", window.frame_time);
    Render_text((ivec2){100, 100}, buff);

    // Render plain text
    Render_text((ivec2){200, 200}, "Hello world!");

    Shader_Bind(tr.cube_shader);
    Camera_View_Projection_To_Shader(tr.cam, tr.cube_shader, "projection");

    Cube_Draw(tr.cube);
}

void TextRendering_OnExit()
{
    Camera_Print_Values(tr.cam);

    VAO_Destroy(tr.text_vao);
    VAO_Destroy(tr.cube);
    Shader_Destroy(&tr.text_shader);
}
