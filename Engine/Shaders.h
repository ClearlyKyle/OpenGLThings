#ifndef __SHADERS_H__
#define __SHADERS_H__

#include "util.h"
#include "Textures.h"

// VertexAttribute - are the (locations = 0) in the vs, for example:
// layout (location = 0) in vec3 position;    "index = 0, name = position"
// layout (location = 1) in vec2 uv;          "index = 1, name = uv"
struct VertexAttribute
{
    GLuint        index;
    const GLchar *name;
};

struct Shader
{
    GLuint shader_id;
    GLuint vs_handle;
    GLuint fs_handle;
    GLuint gs_handle;
};

typedef struct Shader Shader_t;

// Shader Functions
Shader_t Shader_Create(const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
Shader_t Shader_Create2(const char *vs_path, const char *fs_path, const char *gs_path, size_t n, struct VertexAttribute attributes[]);
void     Shader_Destroy(const struct Shader shader);
void     Shader_Bind(const struct Shader shader);

// Uniforms
void Shader_Uniform_Float(struct Shader shader, const char *name, float f);
void Shader_Uniform_Mat4_Floats(struct Shader shader, const char *name, const float *matrix);
void Shader_Uniform_Mat4(struct Shader shader, const char *name, const mat4 m);
void Shader_Uniform_Vec3(struct Shader shader, char *name, vec3 v);
void Shader_Uniform_Vec4(struct Shader shader, char *name, vec4 v);
void Shader_Uniform_Texture2D(struct Shader shader, const char *name, const struct Texture texture);
// void Texture_Uniform(struct Shader shader, const struct Texture texture, const char *uniform, GLuint unit);

#endif // __SHADERS_H__