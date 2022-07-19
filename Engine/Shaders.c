#include "Shaders.h"

#define BUFFER_SIZE 512

static char *_Uniform_type_to_string(GLenum type)
{
    switch (type)
    {
    case GL_INVALID_ENUM:
        return "invalid";
    case GL_FLOAT:
        return "float";
    case GL_FLOAT_VEC2:
        return "vec2";
    case GL_FLOAT_VEC3:
        return "vec3";
    case GL_FLOAT_VEC4:
        return "vec4";
    case GL_DOUBLE:
        return "double";
    case GL_DOUBLE_VEC2:
        return "dvec2";
    case GL_DOUBLE_VEC3:
        return "dvec3";
    case GL_DOUBLE_VEC4:
        return "dvec4";
    case GL_INT:
        return "int";
    case GL_INT_VEC2:
        return "ivec2";
    case GL_INT_VEC3:
        return "ivec3";
    case GL_INT_VEC4:
        return "ivec4";
    case GL_UNSIGNED_INT:
        return "unsigned int";
    case GL_UNSIGNED_INT_VEC2:
        return "uvec2";
    case GL_UNSIGNED_INT_VEC3:
        return "uvec3";
    case GL_UNSIGNED_INT_VEC4:
        return "uvec4";
    case GL_BOOL:
        return "bool";
    case GL_BOOL_VEC2:
        return "bvec2";
    case GL_BOOL_VEC3:
        return "bvec3";
    case GL_BOOL_VEC4:
        return "bvec4";
    case GL_FLOAT_MAT2:
        return "mat2";
    case GL_FLOAT_MAT3:
        return "mat3";
    case GL_FLOAT_MAT4:
        return "mat4";
    case GL_FLOAT_MAT2x3:
        return "mat2x3";
    case GL_FLOAT_MAT2x4:
        return "mat2x4";
    case GL_FLOAT_MAT3x2:
        return "mat3x2";
    case GL_FLOAT_MAT3x4:
        return "mat3x4";
    case GL_FLOAT_MAT4x2:
        return "mat4x2";
    case GL_FLOAT_MAT4x3:
        return "mat4x3";
    case GL_DOUBLE_MAT2:
        return "dmat2";
    case GL_DOUBLE_MAT3:
        return "dmat3";
    case GL_DOUBLE_MAT4:
        return "dmat4";
    case GL_DOUBLE_MAT2x3:
        return "dmat2x3";
    case GL_DOUBLE_MAT2x4:
        return "dmat2x4";
    case GL_DOUBLE_MAT3x2:
        return "dmat3x2";
    case GL_DOUBLE_MAT3x4:
        return "dmat3x4";
    case GL_DOUBLE_MAT4x2:
        return "dmat4x2";
    case GL_DOUBLE_MAT4x3:
        return "dmat4x3";
    case GL_SAMPLER_1D:
        return "sampler1D";
    case GL_SAMPLER_2D:
        return "sampler2D";
    case GL_SAMPLER_3D:
        return "sampler3D";
    case GL_SAMPLER_CUBE:
        return "samplerCube";
    case GL_SAMPLER_1D_SHADOW:
        return "sampler1DShadow";
    case GL_SAMPLER_2D_SHADOW:
        return "sampler2DShadow";
    case GL_SAMPLER_1D_ARRAY:
        return "sampler1DArray";
    case GL_SAMPLER_2D_ARRAY:
        return "sampler2DArray";
    case GL_SAMPLER_1D_ARRAY_SHADOW:
        return "sampler1DArrayShadow";
    case GL_SAMPLER_2D_ARRAY_SHADOW:
        return "sampler2DArrayShadow";
    case GL_SAMPLER_2D_MULTISAMPLE:
        return "sampler2DMS";
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        return "sampler2DMSArray";
    case GL_SAMPLER_CUBE_SHADOW:
        return "samplerCubeShadow";
    case GL_SAMPLER_BUFFER:
        return "samplerBuffer";
    case GL_SAMPLER_2D_RECT:
        return "sampler2DRect";
    case GL_SAMPLER_2D_RECT_SHADOW:
        return "sampler2DRectShadow";
    case GL_INT_SAMPLER_1D:
        return "isampler1D";
    case GL_INT_SAMPLER_2D:
        return "isampler2D";
    case GL_INT_SAMPLER_3D:
        return "isampler3D";
    case GL_INT_SAMPLER_CUBE:
        return "isamplerCube";
    case GL_INT_SAMPLER_1D_ARRAY:
        return "isampler1DArray";
    case GL_INT_SAMPLER_2D_ARRAY:
        return "isampler2DArray";
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
        return "isampler2DMS";
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        return "isampler2DMSArray";
    case GL_INT_SAMPLER_BUFFER:
        return "isamplerBuffer";
    case GL_INT_SAMPLER_2D_RECT:
        return "isampler2DRect";
    case GL_UNSIGNED_INT_SAMPLER_1D:
        return "usampler1D";
    case GL_UNSIGNED_INT_SAMPLER_2D:
        return "usampler2D";
    case GL_UNSIGNED_INT_SAMPLER_3D:
        return "usampler3D";
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
        return "usamplerCube";
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        return "usampler2DArray";
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        return "usampler2DArray";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        return "usampler2DMS";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        return "usampler2DMSArray";
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
        return "usamplerBuffer";
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        return "usampler2DRect";
    case GL_IMAGE_1D:
        return "image1D";
    case GL_IMAGE_2D:
        return "image2D";
    case GL_IMAGE_3D:
        return "image3D";
    case GL_IMAGE_2D_RECT:
        return "image2DRect";
    case GL_IMAGE_CUBE:
        return "imageCube";
    case GL_IMAGE_BUFFER:
        return "imageBuffer";
    case GL_IMAGE_1D_ARRAY:
        return "image1DArray";
    case GL_IMAGE_2D_ARRAY:
        return "image2DArray";
    case GL_IMAGE_2D_MULTISAMPLE:
        return "image2DMS";
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
        return "image2DMSArray";
    case GL_INT_IMAGE_1D:
        return "iimage1D";
    case GL_INT_IMAGE_2D:
        return "iimage2D";
    case GL_INT_IMAGE_3D:
        return "iimage3D";
    case GL_INT_IMAGE_2D_RECT:
        return "iimage2DRect";
    case GL_INT_IMAGE_CUBE:
        return "iimageCube";
    case GL_INT_IMAGE_BUFFER:
        return "iimageBuffer";
    case GL_INT_IMAGE_1D_ARRAY:
        return "iimage1DArray";
    case GL_INT_IMAGE_2D_ARRAY:
        return "iimage2DArray";
    case GL_INT_IMAGE_2D_MULTISAMPLE:
        return "iimage2DMS";
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        return "iimage2DMSArray";
    case GL_UNSIGNED_INT_IMAGE_1D:
        return "uimage1D";
    case GL_UNSIGNED_INT_IMAGE_2D:
        return "uimage2D";
    case GL_UNSIGNED_INT_IMAGE_3D:
        return "uimage3D";
    case GL_UNSIGNED_INT_IMAGE_2D_RECT:
        return "uimage2DRect";
    case GL_UNSIGNED_INT_IMAGE_CUBE:
        return "uimageCube";
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
        return "uimageBuffer";
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
        return "uimage1DArray";
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
        return "uimage2DArray";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
        return "uimage2DMS";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        return "uimage2DMSArray";
    case GL_UNSIGNED_INT_ATOMIC_COUNTER:
        return "atomic_uint";
    default:
        fprintf(stderr, "Uniform Type not in list : %d\n", type);
    }

    return "";
}

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

        Shader_Destroy(&shader);

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

static void _Print_Active_Uniforms_Attribs(const Shader_t shader)
{
    Shader_Bind(shader);

    // https://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
    GLchar name[128];
    GLenum type;
    GLint  size;

    GLint numActiveAttribs  = 0;
    GLint numActiveUniforms = 0;
    glGetProgramiv(shader.shader_id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
    glGetProgramiv(shader.shader_id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

    // glGetProgramInterfaceiv(shader.shader_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    // glGetProgramInterfaceiv(shader.shader_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    for (int i = 0; i < numActiveUniforms; i++)
    {
        glGetActiveUniform(shader.shader_id,
                           i,
                           128,
                           NULL,
                           &size,
                           &type,
                           name);

        const GLint location = glGetUniformLocation(shader.shader_id, name);

        fprintf(stderr, "Uniform : %16s - %3d - %3d - %s\n", name, location, size, _Uniform_type_to_string(type));
    }
    // const GLenum properties[3] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
    // GLuint       values[3];
    // GLchar       nameData[128];

    // for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
    //{
    //     glGetProgramResourceiv(shader.shader_id,
    //                            GL_PROGRAM_INPUT,
    //                            attrib,
    //                            3,
    //                            properties,
    //                            3,
    //                            NULL,
    //                            values);

    //    // nameData.resize(values[0]); // The length of the name.
    //    glGetProgramResourceName(shader.shader_id, GL_PROGRAM_INPUT, attrib, values[0], NULL, nameData);
    //}
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

    _Print_Active_Uniforms_Attribs(shader);

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

    // Detch after successful linkg, and destroy as we dont need them anymore
    glDetachShader(shader.shader_id, shader.fs_handle);
    glDetachShader(shader.shader_id, shader.vs_handle);
    glDeleteShader(shader.vs_handle);
    glDeleteShader(shader.fs_handle);
    shader.vs_handle = 0;
    shader.fs_handle = 0;

    //_Get_Active_Uniforms(shader);
    _Print_Active_Uniforms_Attribs(shader);

    return shader;
}

void Shader_Destroy(struct Shader *shader)
{
    if (shader->vs_handle)
    {
        glDeleteShader(shader->vs_handle);
        shader->vs_handle = 0;
    }

    if (shader->fs_handle)
    {
        glDeleteShader(shader->fs_handle);
        shader->fs_handle = 0;
    }

    if (shader->gs_handle)
    {
        glDeleteShader(shader->gs_handle);
        shader->gs_handle = 0;
    }

    if (shader->shader_id)
    {
        glDeleteProgram(shader->shader_id);
        shader->shader_id = 0;
    }
}

static Shader_t _currently_bound_shader = {0};

void Shader_Bind(const struct Shader shader)
{
    if (shader.shader_id == 0)
        fprintf(stderr, "You are using a shader with id of 0\n");

    glUseProgram(shader.shader_id);

    _currently_bound_shader = shader;
}

// ------------------------------------------------------------------------------------------------
// UNIFORMS

// if a uniform is not used in a shader
// it will be optimized away. Uniforms are defined post-link
// 1- Check if uniform is going to be used
// 2 -Check if uniform is the correct name
#define LOCATION_CHECK(LOCATION, NAME, FUNCTION)                                                                       \
    if ((LOCATION) == GL_INVALID_INDEX)                                                                                \
        fprintf(stderr, _colour_RED "[UNIFORM ERROR] " _colour_COLOUR_X "Error locating uniform name : %s\n", (NAME)); \
    else                                                                                                               \
        FUNCTION;
//fprintf(stderr, _colour_RED "[UNIFORM ERROR] " _colour_COLOUR_X "Error locating uniform name : %s\n", (NAME)); \

void Shader_Uniform_Int(struct Shader shader, const char *name, int i)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1i(location, i));
}

void Shader_Uniform_Float(struct Shader shader, const char *name, float f)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1f(location, f));
}

void Shader_Uniform_Vec3(struct Shader shader, const char *name, const vec3 v)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform3f(location, v[0], v[1], v[2]));
}

void Shader_Uniform_Vec4(struct Shader shader, const char *name, const vec4 v)
{
    // A vec3 is valid to pass to vec4 uniform, easy mistake, check for correct size
    // vec being used, similar for vec3 uniform
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform4f(location, v[0], v[1], v[2], v[3]));
}

void Shader_Uniform_Mat4(struct Shader shader, const char *name, const mat4 matrix)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniformMatrix4fv(location, 1, GL_FALSE, matrix[0]));
}

void Shader_Uniform_Vec2(struct Shader shader, const char *name, const vec2 v)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform2f(location, v[0], v[1]));
}

void Shader_Uniform_Mat4_Floats(struct Shader shader, const char *name, const float *matrix)
{
    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniformMatrix4fv(location, 1, GL_FALSE, matrix));
}

void Shader_Uniform_Texture2D(struct Shader shader, const char *name, const struct Texture texture)
{
    // Shader_Bind(shader);
    // Texture_Bind(texture);

    // Shader_Uniform_Int(shader, name, texture.slot);

    const GLint location = glGetUniformLocation(shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform1i(location, (GLint)texture.slot));
}

// Testing using the static active shader. When a shader is bound, then no need to keep passing
// to every uniform, we set a static shader once and reference it with every subsiquent shader_uniform call
void Uniform_Mat4(const char *name, const mat4 matrix)
{
    const GLint location = glGetUniformLocation(_currently_bound_shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniformMatrix4fv(location, 1, GL_FALSE, matrix[0]));
}

void Uniform_Vec3(const char *name, const vec3 v)
{
    const GLint location = glGetUniformLocation(_currently_bound_shader.shader_id, (const GLchar *)name);
    LOCATION_CHECK(location, name, glUniform3f(location, v[0], v[1], v[2]));
}