#ifndef __TEXT_H__
#define __TEXT_H__

#include "Engine.h"
#include "SDL2/SDL_ttf.h"

typedef struct TextRenderer
{
    TTF_Font *font;
    Shader_t  shader;

    VAO_t  vao;
    GLuint texture;

} TextRenderer;

TextRenderer Text_Renderer_Init(const Shader_t shader, const char *font_file, const int font_size);

// Position and Text can be updated every frame
void Text_Render_Dynamic(const TextRenderer txt_renderer, const ivec2 position, const char *message);

#endif // __TEXT_H__