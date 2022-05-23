#include "SM_With_SP_PL.h"

struct ShadowCubeMap
{
    struct Shader shader;
    GLuint        FBO;
    GLuint        TEX_ID;
    GLsizei       width;
    GLsizei       height;
};

static struct ShadowMapLights
{
    Camera_t    cam;
    Mesh_t      scene;
    Mesh_t      trees;
    FBO_t       post_processing_fbo;
    FBO_t       msaa_fbo;
    Shadowmap_t shadowmap;
    VAO_t       debug_VAO;

    GLuint               pointShadowMapFBO;
    struct ShadowCubeMap cube_map;

    struct Shader shader_default;
    struct Shader shader_framebuffer;
    struct Shader shader_shadowmap;
    struct Shader shader_depth_debug;

    VAO_t         light_VAO;
    struct Shader light_Shader;
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

void ShadowMapLights_Init()
{
    const GLfloat light_vertices[] =
        {//     COORDINATES     //
         -0.1f, -0.1f, 0.1f,
         -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, 0.1f,
         -0.1f, 0.1f, 0.1f,
         -0.1f, 0.1f, -0.1f,
         0.1f, 0.1f, -0.1f,
         0.1f, 0.1f, 0.1f};

    const GLuint light_indices[] =
        {
            // front face
            0, 1, 2, 2, 3, 0,
            // top face
            3, 2, 6, 6, 7, 3,
            // back face
            7, 6, 5, 5, 4, 7,
            // left face
            4, 0, 3, 3, 7, 4,
            // bottom face
            0, 4, 5, 5, 1, 0, // Back to front now.
                              // right face
            1, 5, 6, 6, 2, 1};

    // LIGHT ---------------------------------------------------------
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader light_shader = Shader_Create(
        "../../Examples/18 - Blinn-Phong/lights.vs",
        "../../Examples/18 - Blinn-Phong/lights.fs",
        1,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"}});

    struct VAO light_vao = VAO_Create();

    struct EBO light_ebo = EBO_Create();
    EBO_Buffer(light_ebo, sizeof(light_indices), (void *)light_indices);

    struct VBO light_abo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(light_abo, sizeof(light_vertices), (const GLvoid *)light_vertices);
    VAO_Attr(light_vao, light_abo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    sm.light_Shader = light_shader;
    sm.light_VAO    = light_vao;

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

    struct Shader shader_shadowmap = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowMap.vs",
                                                   "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowMap.fs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"}});

    struct Shader shader_cube_map = Shader_Create2("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowCubeMap.vs",
                                                   "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowCubeMap.fs",
                                                   "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/shadowCubeMap.gs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"}});

    struct Shader shader_depth_debug = Shader_Create("../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/depthdebug.vs",
                                                     "../../Examples/22 - Shadow Maps (Spotlights & Point Lights)/depthdebug.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"},
                                                         {.index = 1, .name = "aTexCoords"}});

    sm.shader_default     = shader_default;
    sm.shader_framebuffer = shader_framebuffer;
    sm.shader_shadowmap   = shader_shadowmap;
    sm.shader_depth_debug = shader_depth_debug;

    glEnable(GL_DEPTH_TEST);  // Enables the Depth Buffer
    glEnable(GL_MULTISAMPLE); // Enables Multisampling
    glEnable(GL_CULL_FACE);   // Enables Cull Facing
    glCullFace(GL_FRONT);     // Keeps front faces
    glFrontFace(GL_CCW);      // Uses counter clock-wise standard

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-11.911216f, 7.288165f, -8.614744f}, cam.position);
    glm_vec3_copy((vec3){0.872691f, -0.406737f, -0.270139f}, cam.orientation);
    cam.pitch = -24.000017f;
    cam.yaw   = -17.199785f;
    sm.cam    = cam;

    // Load in models
    struct Mesh scene = Mesh_Load(shader_default, "../../Examples/res/models/Low Poly Scene/Low-Poly_Models.obj");
    // struct Mesh trees = Mesh_Load(shader_default, "../../Examples/res/models/trees/scene.gltf");
    sm.scene = scene;
    // sm.trees = trees;

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

    // Take care of all the light related things
    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.0f, 2.0f, -15.0f};

    // Matrices needed for the light's perspective
    const float nearPlane = 1.0f;
    const float farPlane  = 100.0f;
    mat4        orthgonalProjection;
    mat4        perspectiveProjection;
    mat4        lightView = GLM_MAT4_IDENTITY_INIT;
    mat4        lightProjection;

    // glm_ortho(-35.0f, 35.0f, -35.0f, 35.0f, nearPlane, farPlane, orthgonalProjection);
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, farPlane, orthgonalProjection);
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, farPlane, perspectiveProjection);
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, lightView);
    glm_mat4_mul(perspectiveProjection, lightView, lightProjection);

    // SHADER UNIFORMS
    Shader_Bind(shader_shadowmap);
    Shader_Uniform_Mat4(shader_shadowmap, "lightProjection", lightProjection);

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader_default, "lightPos", light_position);
    Shader_Uniform_Mat4(shader_default, "lightProjection", lightProjection);
    // Shader_Uniform_Int(shader_default, "shadowMap", 2);
    Shader_Uniform_Int(shader_default, "shadowCubeMap", 2);
    Shader_Uniform_Float(shader_default, "farPlane", farPlane);

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Float(shader_framebuffer, "gamma", 1.0f); // GAMMA value
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);

    // DEBUG DEPTH
    VAO_t debug_VAO = Shadowmap_Debug_Init();
    sm.debug_VAO    = debug_VAO;

    Shader_Bind(shader_depth_debug);
    Shader_Uniform_Int(shader_depth_debug, "depthMap", 0);

    Debug_FBO_Init();

    // CUBEMAP SHADOW MAP
    const GLsizei shadowMapWidth  = (GLsizei)window.width;
    const GLsizei shadowMapHeight = (GLsizei)window.heigh;

    // Framebuffer for Cubemap Shadow Map
    unsigned int pointShadowMapFBO;
    glGenFramebuffers(1, &pointShadowMapFBO);

    // Texture for Cubemap Shadow Map FBO
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sm.cube_map.FBO    = pointShadowMapFBO;
    sm.cube_map.TEX_ID = depthCubemap;
    sm.cube_map.width  = shadowMapWidth;
    sm.cube_map.height = shadowMapHeight;
    sm.cube_map.shader = shader_cube_map;

    // Matrices needed for the light's perspective on all faces of the cubemap
    const float aspect = (float)shadowMapWidth / (float)shadowMapHeight;
    mat4        shadowProj;
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, farPlane, shadowProj);

    mat4 shadowTransforms[6] = {0};

    glm_lookat(light_position, (vec3){light_position[0] + 1.0f, light_position[1], light_position[2]}, (vec3){0.0f, -1.0f, 0.0f}, shadowTransforms[0]);
    glm_lookat(light_position, (vec3){light_position[0] - 1.0f, light_position[1], light_position[2]}, (vec3){0.0f, -1.0f, 0.0f}, shadowTransforms[1]);
    glm_lookat(light_position, (vec3){light_position[0], light_position[1] + 1.0f, light_position[2]}, (vec3){0.0f, 0.0f, 1.0f}, shadowTransforms[2]);
    glm_lookat(light_position, (vec3){light_position[0], light_position[1] - 1.0f, light_position[2]}, (vec3){0.0f, 0.0f, -1.0f}, shadowTransforms[3]);
    glm_lookat(light_position, (vec3){light_position[0], light_position[1], light_position[2] + 1.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowTransforms[4]);
    glm_lookat(light_position, (vec3){light_position[0], light_position[1], light_position[2] - 1.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowTransforms[5]);

    glm_mat4_mul(shadowProj, shadowTransforms[0], shadowTransforms[0]);
    glm_mat4_mul(shadowProj, shadowTransforms[1], shadowTransforms[1]);
    glm_mat4_mul(shadowProj, shadowTransforms[2], shadowTransforms[2]);
    glm_mat4_mul(shadowProj, shadowTransforms[3], shadowTransforms[3]);
    glm_mat4_mul(shadowProj, shadowTransforms[4], shadowTransforms[4]);
    glm_mat4_mul(shadowProj, shadowTransforms[5], shadowTransforms[5]);

    // Export all matrices to shader
    Shader_Bind(shader_cube_map);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[0]", shadowTransforms[0]);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[1]", shadowTransforms[1]);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[2]", shadowTransforms[2]);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[3]", shadowTransforms[3]);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[4]", shadowTransforms[4]);
    Shader_Uniform_Mat4(shader_cube_map, "shadowMatrices[5]", shadowTransforms[5]);
    Shader_Uniform_Vec3(shader_cube_map, "lightPos", light_position);
    Shader_Uniform_Float(shader_cube_map, "farPlane", farPlane);

    Shader_Bind(light_shader);
    mat4 light_model = GLM_MAT4_ZERO_INIT;
    glm_translate_make(light_model, light_position);
    Shader_Uniform_Mat4(light_shader, "model", light_model);
    Shader_Uniform_Vec4(light_shader, "lightColor", light_colour);

    Debug_FBO_Init();
}

void ShadowMapLights_Update()
{
    glEnable(GL_DEPTH_TEST);

    // 1. render scene to shadowmap
    // --------------------------------
    // glViewport(0, 0, sm.shadowmap.width, sm.shadowmap.height);
    // glBindFramebuffer(GL_FRAMEBUFFER, sm.shadowmap.FBO_Id);
    // glClear(GL_DEPTH_BUFFER_BIT);
    //// glCullFace(GL_FRONT);

    //// Render scene with 'shader_cube_map' shader
    // sm.scene.shader = sm.shader_shadowmap;
    // Mesh_Draw(sm.scene);
    // sm.scene.shader = sm.shader_default;

    //// reset
    //// glCullFace(GL_BACK);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);         // Switch back to the default framebuffer
    // glViewport(0, 0, window.width, window.heigh); // Switch back to the default viewport
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. render scene to cube map
    // --------------------------------
    glViewport(0, 0, sm.cube_map.width, sm.cube_map.height);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.cube_map.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glCullFace(GL_FRONT);

    // Render scene with 'shader_cube_map' shader
    sm.scene.shader = sm.cube_map.shader;
    Mesh_Draw(sm.scene);
    sm.scene.shader = sm.shader_default;

    // reset
    // glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);         // Switch back to the default framebuffer
    glViewport(0, 0, window.width, window.heigh); // Switch back to the default viewport
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. render scene as normal
    // -------------------------
    Framebuffer_Draw_Init(sm.msaa_fbo);

    Camera_Inputs(&sm.cam);

    // Send the shadowmap generated to the default shader
    Shader_Bind(sm.shader_default);
    Shader_Uniform_Float(sm.scene.shader, "farPlane", 100.0f);
    Shader_Uniform_Vec3(sm.scene.shader, "camPos", sm.cam.position);
    Camera_View_Projection_To_Shader(sm.cam, sm.scene.shader, "camMatrix");

    // Draw the normal model
    // glActiveTexture(GL_TEXTURE0 + 2);
    // glBindTexture(GL_TEXTURE_2D, sm.shadowmap.tex_Id);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sm.cube_map.TEX_ID); // NOTE: GL_TEXTURE_CUBE_MAP
    Shader_Uniform_Int(sm.scene.shader, "shadowCubeMap", 2);

    Mesh_Draw(sm.scene); // DRAW model normally

    Framebuffer_Update(sm.msaa_fbo, sm.post_processing_fbo);
    Framebuffer_Draw(sm.post_processing_fbo);

    // FBO Debug testing
    // Debug_FBO_Draw(sm.shadowmap.tex_Id);

    //  Draw a Cube to represent the Light source
    Shader_Bind(sm.light_Shader);
    Camera_View_Projection_To_Shader(sm.cam, sm.light_Shader, "camMatrix");
    VAO_Bind(sm.light_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void ShadowMapLights_OnExit()
{
    Camera_Print_Values(sm.cam);

    Mesh_Free(sm.scene);

    Shader_Destroy(&sm.shader_default);
    Shader_Destroy(&sm.shader_framebuffer);
    Shader_Destroy(&sm.shader_shadowmap);
    Shader_Destroy(&sm.shader_depth_debug);
    Shader_Destroy(&sm.cube_map.shader);
}
