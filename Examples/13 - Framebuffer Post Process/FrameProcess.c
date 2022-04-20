#include "FrameProcess.h"

static struct FrameProcess
{
    struct Mesh   model;
    struct Camera cam;

    struct Shader frame_buffer_shader;

    GLuint FBO;
    GLuint rect_VAO;
    GLuint frame_buffer_texture;
} fp;

const GLfloat rectangleVertices[] =
    {
        // Coords    // texCoords
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f};

void FrameProcess_Init()
{
    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/13 - Framebuffer Post Process/default.vs",
                                                 "../../Examples/13 - Framebuffer Post Process/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aColor"},
                                                     {.index = 3, .name = "aTex"}});

    struct Shader shader_frame_buffer = Shader_Create("../../Examples/13 - Framebuffer Post Process/framebuffer.vs",
                                                      "../../Examples/13 - Framebuffer Post Process/framebuffer.fs",
                                                      2,
                                                      (struct VertexAttribute[]){
                                                          {.index = 0, .name = "inPos"},
                                                          {.index = 1, .name = "inTexCoords"}});

    fp.frame_buffer_shader = shader_frame_buffer;

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(shader_frame_buffer);
    glUniform1i(glGetUniformLocation(shader_frame_buffer.shader_id, "screenTexture"), 0);

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);    // Keeps front faces
    glFrontFace(GL_CCW);     // Uses counter clock-wise standard

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){-4.686521f, 11.034898f, 34.881306f}, cam.position);
    glm_vec3_copy((vec3){0.147791f, 0.017452f, -0.988865f}, cam.orientation);
    cam.pitch = 1.000004f;
    cam.yaw   = -81.499794f;

    fp.cam = cam;

    // Loading Models
    struct Mesh crow = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    fp.model         = crow;

    int  i = 10;
    int *j = &i;
    

    // Prepare FRAMEBUFFER rectangle VBO and VAO
    GLuint rect_VAO, rect_VBO;
    glGenVertexArrays(1, &rect_VAO);
    glGenBuffers(1, &rect_VBO);

    glBindVertexArray(rect_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, rect_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    // Create FRAMEBUFFER OBJECT
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create FRAMEBUFFER Texture
    GLuint frame_buffer_texture;
    glGenTextures(1, &frame_buffer_texture);
    glBindTexture(GL_TEXTURE_2D, frame_buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.width, window.heigh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer_texture, 0);

    // Create RENDER Buffer Object
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.width, window.heigh);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // Error checking framebuffer
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    check_that((fbo_status == GL_FRAMEBUFFER_COMPLETE), "Framebuffer error: %d\n", fbo_status);

    fp.rect_VAO             = rect_VAO;
    fp.FBO                  = FBO;
    fp.frame_buffer_texture = frame_buffer_texture;

    glEnable(GL_DEPTH_TEST);
}

void FrameProcess_Update()
{
    // Bind the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fp.FBO);

    // Need to use these after "glBindFramebuffer"
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);            // Specify the color of the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the back buffer and depth buffer
    glEnable(GL_DEPTH_TEST);                            // Enable depth testing since it's disabled when drawing the framebuffer rectangle

    Camera_Inputs(&fp.cam);

    Shader_Uniform_Vec3(fp.model.shader, "camPos", fp.cam.position);
    Camera_View_Projection_To_Shader(fp.cam, fp.model.shader, "camMatrix");

    // Draw the normal model
    Mesh_Draw(fp.model);

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw the framebuffer rectangle
    Shader_Bind(fp.frame_buffer_shader);
    glBindVertexArray(fp.rect_VAO);

    glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded

    glBindTexture(GL_TEXTURE_2D, fp.frame_buffer_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameProcess_OnExit()
{
    Camera_Print_Values(fp.cam);

    Mesh_Free(fp.model);
    glDeleteVertexArrays(1, &fp.rect_VAO);
    glDeleteTextures(1, &fp.frame_buffer_texture);
}
