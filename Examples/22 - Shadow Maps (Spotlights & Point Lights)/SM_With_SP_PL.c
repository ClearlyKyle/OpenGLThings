#include "SM_With_SP_PL.h"

static struct ShadowMapLights
{
    Camera_t    cam;
    Mesh_t      ground;
    Mesh_t      trees;
    FBO_t       post_processing_fbo;
    FBO_t       msaa_fbo;
    Shadowmap_t shadowmap;
    VAO_t       debug_VAO;
} sm;

void ShadowMapLights_Init()
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
    struct Shader shader_default = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/default.vs",
                                                 "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/framebuffer.vs",
                                                     "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/framebuffer.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "inPos"},
                                                         {.index = 1, .name = "inTexCoords"}});

    struct Shader shader_shadowmap = Shader_Create2("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowMap.vs",
                                                    "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowMap.fs",
                                                    "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowMap.gs",
                                                    1,
                                                    (struct VertexAttribute[]){
                                                        {.index = 0, .name = "aPos"}});

    struct Shader shader_cube_map = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowCubeMap.vs",
                                                  "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowCubeMap.fs",
                                                  2,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aTexCoords"}});

    struct Shader shader_depth_debug = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/depthdebug.vs",
                                                     "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/depthdebug.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"},
                                                         {.index = 1, .name = "aTexCoords"}});

    glEnable(GL_DEPTH_TEST);  // Enables the Depth Buffer
    glEnable(GL_MULTISAMPLE); // Enables Multisampling
    glEnable(GL_CULL_FACE);   // Enables Cull Facing
    glCullFace(GL_FRONT);     // Keeps front faces
    glFrontFace(GL_CCW);      // Uses counter clock-wise standard

    // Take care of all the light related things
    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.0f, 10.0f, 0.0f};

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    // glm_vec3_copy((vec3){-10.752992f, 17.550270f, 1.919926f}, cam.position);
    // glm_vec3_copy((vec3){0.991773f, -0.087152f, -0.093755f}, cam.orientation);
    // cam.pitch = -4.999762f;
    // cam.yaw   = -5.400273f;
    sm.cam = cam;

    // Load in models
    struct Mesh ground = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    struct Mesh trees  = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    sm.ground          = ground;
    sm.trees           = trees;

    // POST PROCESSING FBO
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
    post_processing_fbo.VAO   = rectVAO.ID;
    sm.post_processing_fbo    = post_processing_fbo;

    // SHADOWMAP SETUP
    Shadowmap_t shadowmap = Shadowmap_Create(shader_shadowmap);
    sm.shadowmap          = shadowmap;

    // Matrices needed for the light's perspective
    const float farPlane = 100.0f;
    mat4        orthgonalProjection;
    mat4        perspectiveProjection;
    mat4        lightView;
    mat4        lightProjection;

    glm_ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, farPlane, orthgonalProjection);
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, farPlane, perspectiveProjection);
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, lightView);
    glm_mat4_mul(orthgonalProjection, lightView, lightProjection);

    // SHADER UNIFORMS
    Shader_Bind(shader_shadowmap);
    Shader_Uniform_Mat4(shader_shadowmap, "lightProjection", lightProjection);

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColour", light_colour);
    Shader_Uniform_Vec3(shader_default, "lightPos", light_position);
    Shader_Uniform_Mat4(shader_default, "lightProjection", lightProjection);
    Shader_Uniform_Int(shader_default, "shadowMap", 2);

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Float(shader_framebuffer, "gamma", 1.0f); // GAMMA value
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);

    // DEBUG DEPTH
    VAO_t debug_VAO = Shadowmap_Debug_Init();
    sm.debug_VAO    = debug_VAO;

    Shader_Bind(shader_depth_debug);
    Shader_Uniform_Int(shader_depth_debug, "depthMap", 0);
}

void ShadowMapLights_Update()
{
}

void ShadowMapLights_OnExit()
{
}
