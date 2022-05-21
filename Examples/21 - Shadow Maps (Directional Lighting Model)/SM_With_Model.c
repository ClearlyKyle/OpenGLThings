#include "SM_With_Model.h"

static struct ShadowMap
{
    struct Camera cam;

    struct Mesh model;

    struct Shader shader_default;
    struct Shader shader_framebuffer;
    struct Shader shader_shadowmap;
    struct Shader shader_depth_debug;

    FBO_t msaa_fbo;
    FBO_t post_processing_fbo;

    Shadowmap_t shadowmap;
    VAO_t       debug_VAO;
} sm;

static VAO_t Shadowmap_Debug_Init()
{
    const GLfloat depth_debug_verticies[] = {
        // positions        // texture Coords
        -1.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        -1.0f,
        -1.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,
        -1.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    // DEBUG
    struct VAO debug_VAO = VAO_Create();
    struct VBO debug_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(debug_VBO, sizeof(depth_debug_verticies), (const GLvoid *)depth_debug_verticies);
    VAO_Attr(debug_VAO, debug_VBO, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(debug_VAO, debug_VBO, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    return debug_VAO;
}

void ShadowMapModel_Init()
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
    struct Shader shader_default = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Model)/default.vs",
                                                 "../../Examples/21 - Shadow Maps (Directional Lighting Model)/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Model)/framebuffer.vs",
                                                     "../../Examples/21 - Shadow Maps (Directional Lighting Model)/framebuffer.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "inPos"},
                                                         {.index = 1, .name = "inTexCoords"}});

    struct Shader shader_shadowmap = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Model)/shadowMap.vs",
                                                   "../../Examples/21 - Shadow Maps (Directional Lighting Model)/shadowMap.fs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"}});

    struct Shader shader_depth_debug = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Model)/depthdebug.vs",
                                                     "../../Examples/21 - Shadow Maps (Directional Lighting Model)/depthdebug.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"},
                                                         {.index = 1, .name = "aTexCoords"}});

    sm.shader_default     = shader_default;
    sm.shader_framebuffer = shader_framebuffer;
    sm.shader_shadowmap   = shader_shadowmap;
    sm.shader_depth_debug = shader_depth_debug;

    // OPENGL SETTINGS
    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    // glEnable(GL_MULTISAMPLE); // Enables Multisampling
    // glEnable(GL_CULL_FACE);  // Enables Cull Facing
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CCW);

    // Load in models
    struct Mesh crow = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    sm.model         = crow;

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-10.752992f, 17.550270f, 1.919926f}, cam.position);
    glm_vec3_copy((vec3){0.991773f, -0.087152f, -0.093755f}, cam.orientation);
    cam.pitch = -4.999762f;
    cam.yaw   = -5.400273f;
    sm.cam    = cam;

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
    FBO_Add_RBO(&post_processing_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    post_processing_fbo.VAO = rectVAO.ID;
    sm.post_processing_fbo  = post_processing_fbo;

    // SHADOWMAP SETUP
    Shadowmap_t shadowmap = Shadowmap_Create(shader_shadowmap);
    sm.shadowmap          = shadowmap;

    // LIGHT SETUP
    vec4 light_colour     = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position   = {-3.0f, 25.0f, -1.0f};
    mat4 light_orthogonal = GLM_MAT4_IDENTITY_INIT;
    mat4 light_view       = GLM_MAT4_IDENTITY_INIT;
    mat4 light_matrix     = GLM_MAT4_IDENTITY_INIT;

    // glm_vec3_scale(lightPos, 20.0f, lightPos);

    glm_ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f, light_orthogonal);
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, light_view);
    glm_mat4_mul(light_orthogonal, light_view, light_matrix);

    // SHADER UNIFORMS
    Shader_Bind(shader_shadowmap);
    Shader_Uniform_Mat4(shader_shadowmap, "lightMatrix", light_matrix);

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColour", light_colour);
    Shader_Uniform_Vec3(shader_default, "lightPos", light_position);
    Shader_Uniform_Mat4(shader_default, "lightMatrix", light_matrix);
    Shader_Uniform_Int(shader_default, "shadowMap", 2);

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Float(shader_framebuffer, "gamma", 1.0f); // GAMMA value
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);

    // DEBUG DEPTH
    VAO_t debug_VAO = Shadowmap_Debug_Init();
    sm.debug_VAO    = debug_VAO;

    Shader_Bind(shader_depth_debug);
    Shader_Uniform_Int(shader_depth_debug, "depthMap", 0);

    Debug_FBO_Init();
}

void ShadowMapModel_Update()
{
    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);

    // START Draw from light POV
    // Render the Scene for the cameras perspective
    glViewport(0, 0, sm.shadowmap.width, sm.shadowmap.height);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.shadowmap.FBO_Id);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    // BIND Shader for use in shadowmap
    sm.model.shader = sm.shader_shadowmap;
    Mesh_Draw(sm.model); // DRAW model
    sm.model.shader = sm.shader_default;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);         // Switch back to the default framebuffer
    glViewport(0, 0, window.width, window.heigh); // Switch back to the default viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // END Draw from light POV

    Framebuffer_Draw_Init(sm.msaa_fbo);

    Camera_Inputs(&sm.cam);

    // Send the shadowmap generated to the default shader
    Shader_Bind(sm.shader_default);
    Shader_Uniform_Vec3(sm.model.shader, "camPos", sm.cam.position);
    Camera_View_Projection_To_Shader(sm.cam, sm.model.shader, "camMatrix");

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, sm.shadowmap.tex_Id);

    Mesh_Draw(sm.model); // DRAW model normally

    Framebuffer_Update(sm.msaa_fbo, sm.post_processing_fbo);
    Framebuffer_Draw(sm.post_processing_fbo);

    Debug_FBO_Draw(sm.shadowmap.tex_Id);

    // Old debug method:
    // Shader_Bind(sm.shader_depth_debug);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, sm.shadowmap.tex_Id);
    // glBindVertexArray(sm.debug_VAO.ID);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ShadowMapModel_OnExit()
{
    Camera_Print_Values(sm.cam);

    Mesh_Free(sm.model);

    Framebuffer_Destroy(&sm.msaa_fbo);
    Framebuffer_Destroy(&sm.post_processing_fbo);

    Shader_Destroy(&sm.shader_default);
    Shader_Destroy(&sm.shader_framebuffer);
    Shader_Destroy(&sm.shader_shadowmap);
    Shader_Destroy(&sm.shader_depth_debug);

    // Shadowmap_Destroy(sm.shadowmap);
    VAO_Destroy(sm.debug_VAO);
}
