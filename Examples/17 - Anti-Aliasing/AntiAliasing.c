#include "AntiAliasing.h"

static struct AntiAliasing
{
    Camera_t    cam;
    struct Mesh model;
    FBO_t       msaa_fbo;
    FBO_t       post_processing_fbo;
} aa;

void AntiAliasing_Init()
{
    const GLfloat rectangle_vertices[] =
        {
            //  Coords   // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

    // Only use this if you don't have a framebuffer
    // GLuint samples = 4;
    // glfwWindowHint(GLFW_SAMPLES, samples);
    // glEnable(GL_MULTISAMPLE);

    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/17 - Anti-Aliasing/default.vs",
                                                 "../../Examples/17 - Anti-Aliasing/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/17 - Anti-Aliasing/framebuffer.vs",
                                                     "../../Examples/17 - Anti-Aliasing/framebuffer.fs",
                                                     1,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "inPos"},
                                                         {.index = 1, .name = "inTexCoords"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(shader_framebuffer);
    glUniform1i(glGetUniformLocation(shader_framebuffer.shader_id, "screenTexture"), 0);

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-2.898770f, 11.473441f, 6.265410f}, cam.position);
    glm_vec3_copy((vec3){0.384907f, -0.213030f, 0.898034f}, cam.orientation);
    cam.pitch = -12.300006f;
    cam.yaw   = 66.799515f;
    aa.cam    = cam;

    struct VAO rectVAO = VAO_Create();
    struct VBO rectVBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(rectVBO, sizeof(rectangle_vertices), rectangle_vertices);
    VAO_Attr(rectVAO, rectVBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(rectVAO, rectVBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    // FBO_t fbo = FBO_Texture2D(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh);
    // FBO_Add_RBO(fbo);
    // fbo.VAO = rectVAO.ID;
    // aa.fbo  = fbo;

    FBO_t msaa_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D_MULTISAMPLE, window.width, window.heigh, 4);
    FBO_Add_RBO(&msaa_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    msaa_fbo.VAO = rectVAO.ID;
    aa.msaa_fbo  = msaa_fbo;

    FBO_t post_processing_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh, 0);
    FBO_Add_RBO(&post_processing_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    post_processing_fbo.VAO = rectVAO.ID;
    aa.post_processing_fbo  = post_processing_fbo;

    // Load in models
    struct Mesh crow = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    aa.model         = crow;
}

void AntiAliasing_Update()
{
    Framebuffer_Draw_Init(aa.post_processing_fbo);
    // Framebuffer_Draw_Init(aa.msaa_fbo);

    Camera_Inputs(&aa.cam);

    // TODO : Move this into the model Draw call
    Shader_Uniform_Vec3(aa.model.shader, "camPos", aa.cam.position);
    Camera_View_Projection_To_Shader(aa.cam, aa.model.shader, "camMatrix");

    Mesh_Draw(aa.model);

    // Framebuffer_Update(aa.msaa_fbo, aa.post_processing_fbo);

    // Bind the default framebuffer
    Framebuffer_Draw(aa.post_processing_fbo);
}

void AntiAliasing_OnExit()
{
    Camera_Print_Values(aa.cam);

    Mesh_Free(aa.model);
    Framebuffer_Destroy(aa.msaa_fbo);
    Framebuffer_Destroy(aa.post_processing_fbo);
}
