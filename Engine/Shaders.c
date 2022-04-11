#include "Shaders.h"

#define BUFFER_SIZE 512

static void _Validate_Shader(GLuint handle, const char *file)
{
    // Check link status
    GLint compiled = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE)
    {
        // char buf[512];
        //  snprintf(buf, 512, "[%s, %s]", vertex_shader_path, fragment_shader_path);

        GLint loglen;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglen);

        char *logtext = (char *)calloc(1, loglen);
        glGetShaderInfoLog(handle, loglen, NULL, logtext);
        glDeleteShader(handle);

        fprintf(stderr, "[Shader %s error] %s\n%s\n", "COMPILE", file, logtext);

        exit(1);
    }
}

static void _Validate_Program(struct Shader shader, GLenum status, const char *vertex_shader_path, const char *fragment_shader_path)
{
    // Check link status
    GLint linked = 0;
    glGetProgramiv(shader.shader_id, status, &linked);

    if (linked == GL_FALSE)
    {
        char buf[512];
        snprintf(buf, 512, "(%s, %s)", vertex_shader_path, fragment_shader_path);

        GLint loglen;
        glGetProgramiv(shader.shader_id, GL_INFO_LOG_LENGTH, &loglen);

        char *logtext = (char *)calloc(1, loglen);
        glGetProgramInfoLog(shader.shader_id, loglen, NULL, logtext);

        fprintf(stderr, "[Shader %s error] %s\n%s\n", "COMPILE", buf, logtext);

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
    check_that(len > 0, "File length needs to be greater than 0!\n");

    // Get text from file
    fseek(f, 0, SEEK_SET);
    char *file_text = calloc(1, len);
    check_that(file_text != NULL, "Error loading text from file : %s\n", path);

    fread(file_text, 1, len, f);
    check_that(strlen(file_text) > 0, "file_text len failed\n");

    // CLOSE file pointer
    fclose(f);

    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar *const *)&file_text, (const GLint *)&len);
    glCompileShader(handle);

    _Validate_Shader(handle, path);

    free(file_text);
    return handle;
}

static void _Get_Active_Uniforms(struct Shader shader)
{
    // The index returned from these GLint functions gets passed to glEnableVertexAttribArray during rendering.
    // GLint position_attrib_index = glGetAttribLocation(shader.shader_id, "position");
    // GLint texcoord_attrib_index = glGetAttribLocation(shader.shader_id, "texcoord");
    // GLint normal_attrib_index = glGetAttribLocation(shader.shader_id, "normal");
    // GLint color_attrib_index = glGetAttribLocation(shader.shader_id, "color");

    GLint num_uniforms = 0;
    glGetProgramiv(shader.shader_id, GL_ACTIVE_UNIFORMS, &num_uniforms);
    GLint max_char_len = 0;
    glGetProgramiv(shader.shader_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_char_len);

    if (num_uniforms > 0 && max_char_len > 0)
    {
        char *buffer = (char *)malloc(sizeof(char) * max_char_len);
        for (GLint i = 0; i < num_uniforms; i++)
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

    _Validate_Program(shader, GL_LINK_STATUS, vertex_shader_path, fragment_shader_path);

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

// ------------------------------------------------------------------------------------------------
// UNIFORMS

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

void Shader_Uniform_Mat4_Floats(struct Shader shader, const char *name, const float *matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.shader_id, name), 1, GL_FALSE, matrix);
}

void Shader_Uniform_Texture2D(struct Shader shader, char *name, const struct Texture texture, GLuint n)
{
    glActiveTexture(GL_TEXTURE0 + n);
    Texture_Bind(texture);

    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    if (location < 0)
        fprintf(stderr, "Shader_Uniform_Texture2D might have an error =D\n");
    else
        glUniform1i(location, n);
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