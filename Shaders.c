#include "Shaders.h"

#define BUFFER_SIZE 512

void validate_shader(GLuint shader, const char *file)
{
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if (length > 0)
    {
        fprintf(stderr, "Shader %d(%s) compile error: %s\n", shader, (file ? file : ""), buffer);
    }
}

bool validate_program(GLuint program)
{
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);

    if (length > 0)
    {
        fprintf(stderr, "Program %d link error: %s\n", program, buffer);
        return false;
    }

    return true;
}
static GLint _compile(char *path, GLenum type)
{
    FILE *f;
    char *text;
    long len;

    f = fopen(path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "error loading shader at %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    text = calloc(1, len);
    assert(text != NULL);
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar *const *)&text, (const GLint *)&len);
    glCompileShader(handle);

    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    // Check OpenGL logs if compilation failed
    if (compiled == 0)
    {
        _log_and_fail(handle, "compiling", path, glGetShaderInfoLog, glGetShaderiv);
    }

    free(text);
    return handle;
}

struct Shader Shader_Create(char *vertex_shader_path, char *fragment_shader_path, size_t n, struct VertexAttr attributes[])
{
    struct Shader self;
    self.vs_handle = _compile(vertex_shader_path, GL_VERTEX_SHADER);
    self.fs_handle = _compile(fragment_shader_path, GL_FRAGMENT_SHADER);
    self.shader_id = glCreateProgram();

    // Link shader program
    glAttachShader(self.shader_id, self.vs_handle);
    glAttachShader(self.shader_id, self.fs_handle);

    // Bind vertex attributes
    for (size_t i = 0; i < n; i++)
    {
        glBindAttribLocation(self.shader_id, attributes[i].index, attributes[i].name);
    }

    glLinkProgram(self.shader_id);

    // Check link status
    GLint linked;
    glGetProgramiv(self.shader_id, GL_LINK_STATUS, &linked);

    if (linked == 0)
    {
        char buf[512];
        snprintf(buf, 512, "[%s, %s]", vertex_shader_path, fragment_shader_path);
        _log_and_fail(self.shader_id, "linking", buf, glGetProgramInfoLog, glGetProgramiv);
    }

    return self;
}