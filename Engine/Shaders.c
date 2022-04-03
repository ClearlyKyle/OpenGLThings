#include "Shaders.h"

#define BUFFER_SIZE 512

#define assert__(x) for (; !(x); assert(x))

static void _Validate_Shader(GLuint shader, const char *file)
{
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if (length > 0)
    {
        printf("Shader %d(%s) compile error: %s\n", shader, (file ? file : ""), buffer);
    }
}

static void _Validate_Program(struct Shader shader, const char *vertex_shader_path, const char *fragment_shader_path)
{
    // Check link status
    GLint linked = 0;
    glGetProgramiv(shader.shader_id, GL_LINK_STATUS, &linked);

    if (linked == GL_FALSE)
    {
        char buf[512];
        // snprintf(buf, 512, "[%s, %s]", vertex_shader_path, fragment_shader_path);

        GLint loglen;
        glGetProgramiv(shader.shader_id, GL_INFO_LOG_LENGTH, &loglen);

        char *logtext = (char *)calloc(1, loglen);
        glGetProgramInfoLog(shader.shader_id, loglen, NULL, logtext);
        fprintf(stderr, "[Shader Program Error] %s shader at %s:\n%s", "linking", buf, logtext);

        Shader_Destroy(shader);

        exit(1);
    }
}

static GLint _Compile_Shader(const char *path, GLenum type)
{
    FILE *f;
    fopen_s(&f, path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "[Error] loading shader at : %s\n", path);
        exit(1);
    }

    // Get length of file
    fseek(f, 0, SEEK_END);
    const long len = ftell(f);
    assert__(len > 0)
    {
        fprintf(stderr, "File length needs to be greater than 0!\n");
    };

    // Get text from file
    fseek(f, 0, SEEK_SET);
    char *file_text = calloc(1, len);
    assert__(file_text != NULL)
    {
        fprintf(stderr, "Error loading text from file : %s\n", path);
    };

    fread(file_text, 1, len, f);
    assert__(strlen(file_text) > 0);

    // CLOSE file pointer
    fclose(f);

    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar *const *)&file_text, (const GLint *)&len);
    glCompileShader(handle);

    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    // Check OpenGL logs if compilation failed
    if (compiled == 0)
    {
        printf("Error with compiling!");
        free(file_text);
        exit(1);
    }

    free(file_text);
    return handle;
}

static void _Get_Active_Uniforms(struct Shader shader)
{
    GLint num_uniforms = 0;
    glGetProgramiv(shader.shader_id, GL_ACTIVE_UNIFORMS, &num_uniforms);
    GLint max_char_len = 0;
    glGetProgramiv(shader.shader_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_char_len);

    if (num_uniforms > 0 && max_char_len > 0)
    {
        char *buffer = (char *)malloc(sizeof(char) * max_char_len);
        for (GLuint i = 0; i < num_uniforms; i++)
        {
            GLsizei length, size;
            GLenum type;
            glGetActiveUniform(shader.shader_id, i, max_char_len, &length, &size, &type, buffer);

            GLuint var_location = glGetUniformLocation(shader.shader_id, buffer);
        }
        free(buffer);
    }
}

struct Shader Shader_Create(const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[])
{
    struct Shader shader;
    shader.vs_handle = _Compile_Shader(vertex_shader_path, GL_VERTEX_SHADER);
    shader.fs_handle = _Compile_Shader(fragment_shader_path, GL_FRAGMENT_SHADER);
    shader.shader_id = glCreateProgram();

    _Validate_Shader(shader.vs_handle, vertex_shader_path);
    _Validate_Shader(shader.fs_handle, fragment_shader_path);

    // Link shader program
    glAttachShader(shader.shader_id, shader.vs_handle);
    glAttachShader(shader.shader_id, shader.fs_handle);

    // Bind vertex attributes
    if (attributes != NULL)
    {
        for (size_t i = 0; i < n; i++)
        {
            glBindAttribLocation(shader.shader_id, attributes[i].index, attributes[i].name);
        }
    }

    glLinkProgram(shader.shader_id);

    _Validate_Program(shader, vertex_shader_path, fragment_shader_path);

    //_Get_Active_Uniforms(shader);

    // Detch after successful linkg, and destroy as we dont need them anymore
    glDetachShader(shader.shader_id, shader.fs_handle);
    glDetachShader(shader.shader_id, shader.vs_handle);
    glDeleteShader(shader.vs_handle);
    glDeleteShader(shader.fs_handle);

    return shader;
}

void Shader_Destroy(const struct Shader shader)
{
    glDeleteProgram(shader.shader_id);
    glDeleteShader(shader.vs_handle);
    glDeleteShader(shader.fs_handle);
}

void Shader_Bind(const struct Shader shader)
{
    glUseProgram(shader.shader_id);
}

void Shader_Uniform_Float(struct Shader shader, const char *name, float f)
{
    glUniform1f(glGetUniformLocation(shader.shader_id, name), f);
}

void Shader_Uniform_Vec3(struct Shader shader, char *name, vec3 v)
{
    glUniform3f(glGetUniformLocation(shader.shader_id, name), v[0], v[1], v[2]);
}

void Shader_Uniform_Vec4(struct Shader shader, char *name, vec4 v)
{
    // A vec3 is valid to pass to vec4 uniform, easy mistake, check for correct size
    // vec being used, similar for vec3 uniform
    glUniform4f(glGetUniformLocation(shader.shader_id, name), v[0], v[1], v[2], v[3]);
}

void Shader_Uniform_Mat4(struct Shader shader, const char *name, const mat4 matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.shader_id, name), 1, GL_FALSE, matrix[0]);
}

void Shader_Uniform_Texture2D(struct Shader shader, char *name, const struct Texture texture, GLuint n)
{
    glActiveTexture(GL_TEXTURE0 + n);
    Texture_Bind(texture);
    glUniform1i(glGetUniformLocation(shader.shader_id, (const GLchar *)name), n);
}

// Move this to Shder file?
void Texture_Uniform(struct Shader shader, const struct Texture texture, const char *uniform, GLuint unit)
{
    // Shader needs to be activated before changing the value of a uniform
    Shader_Bind(shader);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texture.type, texture.ID);

    // Sets the value of the uniform
    glUniform1i(glGetUniformLocation(shader.shader_id, (const GLchar *)uniform), unit);
}