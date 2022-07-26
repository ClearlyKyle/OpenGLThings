#include "Text.h"

TextRenderer Text_Renderer_Init(const Shader_t shader, const char *font_file, const int font_size)
{
    // Initialise TTF
    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        window.quit = true;
        return;
    }

    // Open the font
    TTF_Font *font = TTF_OpenFont(font_file, font_size);
    if (font == NULL)
    {
        fprintf(stderr, "Error using TTF_OpenFont: %s\nFont Path: %s", TTF_GetError(), font_file);
        window.quit = true;
        return;
    }

    TextRenderer tr = {0};
    tr.font         = font;
    tr.shader       = shader;

    return tr;
}

void Text_Render_Dynamic(const TextRenderer txt_renderer, const ivec2 position, const char *message)
{
    Shader_Bind(txt_renderer.shader);

    SDL_Surface *surface_message = TTF_RenderText_Blended(txt_renderer.font, message, (SDL_Color){255, 255, 255});
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

TextRenderer Text_Render_Static_Init(const Shader_t shader,
                                     const char    *font_file,
                                     const int      font_size,
                                     const ivec2    position,
                                     const char    *message)
{
    TextRenderer tr = Text_Renderer_Init(shader, font_file, font_size);

    Shader_Bind(tr.shader);

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

    tr.vao     = vao;
    tr.texture = texture;

    return tr;
}

TextRenderer Text_Render_Static_Draw(const TextRenderer txt_renderer)
{
    VAO_Bind(txt_renderer.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, txt_renderer.texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}