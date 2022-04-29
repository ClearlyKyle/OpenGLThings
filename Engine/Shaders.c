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

static void _Validate_Program(struct Shader shader, GLenum status, const char *vs_path, const char *fs_path, const char *gs_path)
{
    // Check link status
    GLint linked = 0;
    glGetProgramiv(shader.shader_id, status, &linked);

    if (linked == GL_FALSE)
    {
        char file_paths[512]; // Just joining file paths for error printing information...
        snprintf(file_paths, 512, "vs : %s\nfs : %s\ngs : %s", vs_path, fs_path, gs_path);

        GLint loglen;
        glGetProgramiv(shader.shader_id, GL_INFO_LOG_LENGTH, &loglen);

        char *logtext = (char *)calloc(1, loglen);
        glGetProgramInfoLog(shader.shader_id, loglen, NULL, logtext);

        fprintf(stderr, "[Shader %s error] \n%s\n%s\n", "COMPILE", file_paths, logtext);

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
            GLenum  type;
            glGetActiveUniform(shader.shader_id, i, max_char_len, &length, &size, &type, buffer);

            GLuint var_location = glGetUniformLocation(shader.shader_id, buffer);
        }
        free(buffer);
    }
}

struct Shader Shader_Create2(const char *vs_path, const char *fs_path, const char *gs_path, size_t n, struct VertexAttribute attributes[])
{
    // TODO : Chest file extension for each shader

    struct Shader shader = {0};

    shader.vs_handle = _Compile_Shader(vs_path, GL_VERTEX_SHADER);
    shader.fs_handle = _Compile_Shader(fs_path, GL_FRAGMENT_SHADER);
    shader.shader_id = glCreateProgram();

    _Validate_Shader(shader.vs_handle, vs_path);
    _Validate_Shader(shader.fs_handle, fs_path);

    // Link shader program
    glAttachShader(shader.shader_id, shader.vs_handle);
    glAttachShader(shader.shader_id, shader.fs_handle);

    if (gs_path != NULL)
    {
        shader.gs_handle = _Compile_Shader(gs_path, GL_GEOMETRY_SHADER);
        _Validate_Shader(shader.gs_handle, gs_path);
        glAttachShader(shader.shader_id, shader.gs_handle);
    }

    // Bind vertex attributes
    if (attributes != NULL)
    {
        for (size_t i = 0; i < n; i++)
        {
            glBindAttribLocation(shader.shader_id, attributes[i].index, attributes[i].name);
        }
    }

    glLinkProgram(shader.shader_id);

    _Validate_Program(shader, GL_LINK_STATUS, vs_path, fs_path, gs_path);

    //_Get_Active_Uniforms(shader);

    // Detch after successful linkg, and destroy as we dont need them anymore
    glDetachShader(shader.shader_id, shader.fs_handle);
    glDetachShader(shader.shader_id, shader.vs_handle);
    glDetachShader(shader.shader_id, shader.gs_handle);
    glDeleteShader(shader.vs_handle);
    glDeleteShader(shader.fs_handle);
    glDeleteShader(shader.gs_handle);
    shader.vs_handle = 0;
    shader.fs_handle = 0;
    shader.gs_handle = 0;

    return shader;
}

struct Shader Shader_Create(const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[])
{
    struct Shader shader = {0};
    shader.vs_handle     = _Compile_Shader(vertex_shader_path, GL_VERTEX_SHADER);
    shader.fs_handle     = _Compile_Shader(fragment_shader_path, GL_FRAGMENT_SHADER);
    shader.shader_id     = glCreateProgram();

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

    char file_paths[512];
    snprintf(file_paths, 512, "vs : %s\nfs : %s", vertex_shader_path, fragment_shader_path);

    _Validate_Program(shader, GL_LINK_STATUS, vertex_shader_path, fragment_shader_path, NULL);

    //_Get_Active_Uniforms(shader);

    // Detch after successful linkg, and destroy as we dont need them anymore
    glDetachShader(shader.shader_id, shader.fs_handle);
    glDetachShader(shader.shader_id, shader.vs_handle);
    glDeleteShader(shader.vs_handle);
    glDeleteShader(shader.fs_handle);
    shader.vs_handle = 0;
    shader.fs_handle = 0;

    return shader;
}

void Shader_Destroy(const struct Shader shader)
{
    if (shader.vs_handle)
        glDeleteShader(shader.vs_handle);

    if (shader.fs_handle)
        glDeleteShader(shader.fs_handle);

    if (shader.gs_handle)
        glDeleteShader(shader.gs_handle);

    glDeleteProgram(shader.shader_id);

    CHECK_GL_ERRORS;
}

void Shader_Bind(const struct Shader shader)
{
    glUseProgram(shader.shader_id);
}

// ------------------------------------------------------------------------------------------------
// UNIFORMS

#define LOCATION_CHECK(LOCATION, NAME, FUNCTION)                                                                       \
    if ((LOCATION) < 0)                                                                                                \
        fprintf(stderr, _colour_RED "[UNIFORM ERROR] " _colour_COLOUR_X "Error locating uniform name : %s\n", (NAME)); \
    else                                                                                                               \
        CHECK_GL_FUNC(FUNCTION);

void Shader_Uniform_Int(struct Shader shader, const char *name, int i)
{
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1i(location, i));
}

void Shader_Uniform_Float(struct Shader shader, const char *name, float f)
{
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1f(location, f));
}

void Shader_Uniform_Vec3(struct Shader shader, char *name, vec3 v)
{
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform3f(location, v[0], v[1], v[2]));
}

void Shader_Uniform_Vec4(struct Shader shader, char *name, vec4 v)
{
    // A vec3 is valid to pass to vec4 uniform, easy mistake, check for correct size
    // vec being used, similar for vec3 uniform
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform4f(location, v[0], v[1], v[2], v[3]));
}

void Shader_Uniform_Mat4(struct Shader shader, const char *name, const mat4 matrix)
{
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniformMatrix4fv(location, 1, GL_FALSE, matrix[0]));
}

void Shader_Uniform_Mat4_Floats(struct Shader shader, const char *name, const float *matrix)
{
    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniformMatrix4fv(location, 1, GL_FALSE, matrix));
}

void Shader_Uniform_Texture2D(struct Shader shader, const char *name, const struct Texture texture)
{
    // Shader_Bind(shader);
    // Texture_Bind(texture);

    // Shader_Uniform_Int(shader, name, texture.slot);

    const GLuint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1i(location, (GLint)texture.slot));
}