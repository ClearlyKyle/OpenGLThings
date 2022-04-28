#include "ShadowMapsDL.h"

static struct ShadowMap
{
    struct Camera cam;

    struct Mesh model;

    FBO_t msaa_fbo;
    FBO_t post_processing_fbo;
    FBO_t shadowmap_fbo;

    float gamma;
} sm;

void ShadowMap_Init()
{
    const GLfloat rectangle_verticies[] =
        {
            //  Coords   // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/21 - Shadow Maps DL/default.vs",
                                                 "../../Examples/21 - Shadow Maps DL/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/21 - Shadow Maps DL/framebuffer.vs",
                                                     "../../Examples/21 - Shadow Maps DL/framebuffer.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "inPos"},
                                                         {.index = 1, .name = "inTexCoords"}});

    struct Shader shader_shadowmap = Shader_Create("../../Examples/21 - Shadow Maps DL/shadowMap.vs",
                                                   "../../Examples/21 - Shadow Maps DL/shadowMap.fs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {-0.5f, 0.5f, 0.5f};

    // Controls the gamma function
    sm.gamma = 1.0f;

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(shader_framebuffer);
    glUniform1i(glGetUniformLocation(shader_framebuffer.shader_id, "screenTexture"), 0);
    Shader_Uniform_Float(shader_framebuffer, "gamma", sm.gamma);

    glEnable(GL_DEPTH_TEST);  // Enables the Depth Buffer
    glEnable(GL_MULTISAMPLE); // Enables Multisampling
    glEnable(GL_CULL_FACE);   // Enables Cull Facing
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    // glm_vec3_copy((vec3){-11.172512f, 21.876404f, 33.018307f}, cam.position);
    // glm_vec3_copy((vec3){0.302723f, -0.265558f, -0.915335f}, cam.orientation);
    // cam.pitch = -15.400095f;
    // cam.yaw   = -71.699715f;
    sm.cam = cam;

    struct VAO rectVAO = VAO_Create();
    struct VBO rectVBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(rectVBO, sizeof(rectangle_verticies), rectangle_verticies);
    VAO_Attr(rectVAO, rectVBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(rectVAO, rectVBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    // MSAA FBO
    FBO_t msaa_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D_MULTISAMPLE, window.width, window.heigh, 4);
    FBO_Add_RBO(&msaa_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    msaa_fbo.VAO = rectVAO.ID;
    sm.msaa_fbo  = msaa_fbo;

    // POST PROCESSING FBO
    FBO_t post_processing_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh, 0);
    // FBO_Add_RBO(&post_processing_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    post_processing_fbo.VAO = rectVAO.ID;
    sm.post_processing_fbo  = post_processing_fbo;

    // SHADOWMAP FBO
    GLsizei shadowMap_width = 2048, shadowmap_height = 2048;
    FBO_t   shadowmap_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, shadowMap_width, shadowmap_height, 0);
    sm.shadowmap_fbo      = shadowmap_fbo;

    // glBindTexture(GL_TEXTURE_2D, shadowmap_fbo.FBO_tex);
    // const float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

    FBO_Bind(shadowmap_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap_fbo.FBO_tex, 0);
    // Needed since we don't touch the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shadowmap_fbo.VAO = rectVAO.ID;
    sm.shadowmap_fbo  = shadowmap_fbo;

    // Matrices needed for the light's perspective
    mat4 orthogonal_projection;
    mat4 light_view;
    mat4 light_projection;

    glm_vec3_scale(lightPos, 20.0f, lightPos);

    glm_ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f, orthogonal_projection);
    glm_lookat(lightPos, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, light_view);
    glm_mat4_mul(orthogonal_projection, light_view, light_projection);

    Shader_Bind(shader_shadowmap);
    Shader_Uniform_Mat4(shader_shadowmap, "lightProjection", light_projection);

    // Load in models
    struct Mesh crow = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    sm.model         = crow;
}

void ShadowMap_Update()
{
    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);

    // Preparations for the Shadow Map
    glViewport(0, 0, sm.shadowmap_fbo.width, sm.shadowmap_fbo.height);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.shadowmap_fbo.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    Mesh_Draw(sm.model);

    // Switch back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Switch back to the default viewport
    glViewport(0, 0, window.width, window.heigh);
    // Bind the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, sm.post_processing_fbo.FBO);
    // Specify the color of the background
    // glClearColor((GLfloat)pow(0.07, sm.gamma), (GLfloat)pow(0.13, sm.gamma), (GLfloat)(0.17, sm.gamma), 1.0f);
    // glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth testing since it's disabled when drawing the framebuffer rectangle
    glEnable(GL_DEPTH_TEST);

    Shader_Uniform_Vec3(sm.model.shader, "camPos", sm.cam.position);
    Camera_View_Projection_To_Shader(sm.cam, sm.model.shader, "camMatrix");

    // Bind the Shadow Map
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, sm.shadowmap_fbo.FBO);
    glUniform1i(glGetUniformLocation(sm.shadowmap_fbo.shader.shader_id, "shadowMap"), 2);

    // Draw the normal model
    Mesh_Draw(sm.model);

    Framebuffer_Update(sm.msaa_fbo, sm.post_processing_fbo);

    // Bind the default framebuffer
    Framebuffer_Draw(sm.post_processing_fbo);
}

void ShadowMap_OnExit()
{
    Camera_Print_Values(sm.cam);

    Mesh_Free(sm.model);

    Framebuffer_Destroy(sm.msaa_fbo);
    Framebuffer_Destroy(sm.post_processing_fbo);
    Framebuffer_Destroy(sm.shadowmap_fbo);
}
