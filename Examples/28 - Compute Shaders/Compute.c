#include "Compute.h"

// You define a fixed size of the work to complete in terms of number of jobs
// - the count of work groups. OpenGL calls this the global work group. This
// will be i.e. the total dimensions of a 2d image to write.

// https://antongerdelan.net/opengl/compute.html

static struct Compute
{
    GLuint computeProgram;
    GLuint screenShaderProgram;
    GLuint screenTex;
    GLuint VAO;

    GLuint indices_count;

    GLuint num_groups_x;
    GLuint num_groups_y;
    GLuint num_groups_z;

} cs;

char *Read_File(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        const long length = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *)calloc(1, length);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
        return buffer;
    }
    else
    {
        fprintf(stderr, "Error opening %s\n", filename);
        fclose(f);
        return NULL;
    }
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

void Compute_Init()
{
    const GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

    const GLuint indices[] = {
        0, 2, 1,
        0, 3, 2};

    cs.indices_count = sizeof(indices) / sizeof(indices[0]);

    const char *screenVertexShaderSource   = Read_File("../../Examples/28 - Compute Shaders/screen.vs");
    const char *screenFragmentShaderSource = Read_File("../../Examples/28 - Compute Shaders/screen.fs");
    const char *screenComputeShaderSource  = Read_File("../../Examples/28 - Compute Shaders/screen.comp");

    GLuint VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
    glVertexArrayElementBuffer(VAO, EBO);

    GLuint screenTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(screenTex, 1, GL_RGBA32F, window.width, window.heigh);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
    glCompileShader(screenVertexShader);
    _Validate_Shader(screenVertexShader, screenVertexShaderSource);

    GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
    glCompileShader(screenFragmentShader);
    _Validate_Shader(screenFragmentShader, screenFragmentShaderSource);

    GLuint screenShaderProgram = glCreateProgram();
    glAttachShader(screenShaderProgram, screenVertexShader);
    glAttachShader(screenShaderProgram, screenFragmentShader);
    glLinkProgram(screenShaderProgram);

    glDeleteShader(screenVertexShader);
    glDeleteShader(screenFragmentShader);

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL);
    glCompileShader(computeShader);
    _Validate_Shader(computeShader, screenComputeShaderSource);

    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    cs.computeProgram      = computeProgram;
    cs.screenShaderProgram = screenShaderProgram;
    cs.screenTex           = screenTex;
    cs.VAO                 = VAO;

    cs.num_groups_x = (GLuint)ceil((double)window.width / 8.0); // The number of work groups to be launched in the X dimension.
    cs.num_groups_y = (GLuint)ceil((double)window.heigh / 4.0); // number of work groups to be launched in the Y dimension.
    cs.num_groups_z = 1;                                        // The number of work groups to be launched in the Z dimension.

    GLint work_grp_cnt[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
    printf("Max work groups per compute shader - x: %d y: %d z: %d\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    GLint work_grp_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    printf("Max work group sizes - x: %4d y: %4d z: %4d\n", work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    GLint work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("Max invocations count per work group: %d\n", work_grp_inv);
}

void Compute_Update()
{
    glUseProgram(cs.computeProgram);
    glDispatchCompute(cs.num_groups_x, cs.num_groups_y, cs.num_groups_z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glUseProgram(cs.screenShaderProgram);

    glBindTextureUnit(0, cs.screenTex);
    glUniform1i(glGetUniformLocation(cs.screenShaderProgram, "screen"), 0);

    glBindVertexArray(cs.VAO);
    glDrawElements(GL_TRIANGLES, cs.indices_count, GL_UNSIGNED_INT, 0);
}

void Compute_OnExit()
{
    // TODO : Clean up compute shaders
}
