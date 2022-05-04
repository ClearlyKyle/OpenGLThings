#include "SM_With_Cubes.h"

static struct SMCubes
{
    Camera_t cam;

    struct Shader plane_shader;
    struct Shader cube_shader;
    struct Shader shader_shadowmap;
    struct Shader shader_depth_debug;

    struct Shadowmap shadowmap;

    Texture_t plane_tex;
    Texture_t cube_tex;

    mat4 light_projection;
    mat4 plane;
    mat4 cube1;
    mat4 cube2;
    mat4 cube3;

    GLuint plane_VAO;
    GLuint cube_VAO;
    GLuint debug_VAO;
} sm;

static void Draw_Plane(GLuint plane_VAO)
{
    glBindVertexArray(plane_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0); // Unbind
}

static void Draw_Cube(GLuint cube_VAO)
{
    glBindVertexArray(cube_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0); // Unbind
}

static void Render_Depth_Debug_Map(GLuint debug_VAO)
{
    glBindVertexArray(debug_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // glBindVertexArray(0);
}

void ShadowMapCubes_Init()
{
    const GLfloat plane_verticies[] = {
        // positions            // normals         // texcoords
        25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,

        25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
        25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f};

    const GLfloat cube_vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

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

    // Generates shaders
    struct Shader shader_plane = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/plane_shader.vs",
                                               "../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/plane_shader.fs",
                                               3,
                                               (struct VertexAttribute[]){
                                                   {.index = 0, .name = "aPos"},
                                                   {.index = 1, .name = "aNormal"},
                                                   {.index = 2, .name = "aTex"}});

    struct Shader shader_cube = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/cube_shader.vs",
                                              "../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/cube_shader.fs",
                                              3,
                                              (struct VertexAttribute[]){
                                                  {.index = 0, .name = "aPos"},
                                                  {.index = 1, .name = "aNormal"},
                                                  {.index = 2, .name = "aTex"}});

    struct Shader shader_shadowmap = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/shadowMap.vs",
                                                   "../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/shadowMap.fs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"}});

    struct Shader shader_depth_debug = Shader_Create("../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/depthdebug.vs",
                                                     "../../Examples/21 - Shadow Maps (Directional Lighting Cubes)/depthdebug.fs",
                                                     2,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"},
                                                         {.index = 1, .name = "aTexCoords"}});

    glEnable(GL_DEPTH_TEST);

    //// Enables Cull Facing
    // glEnable(GL_CULL_FACE);
    //// Keeps front faces
    // glCullFace(GL_FRONT);
    //// Uses counter clock-wise standard
    // glFrontFace(GL_CCW);

    sm.plane_shader       = shader_plane;
    sm.cube_shader        = shader_cube;
    sm.shader_shadowmap   = shader_shadowmap;
    sm.shader_depth_debug = shader_depth_debug;

    // PLANE
    struct VAO plane_VAO = VAO_Create();
    struct VBO plane_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(plane_VBO, sizeof(plane_verticies), (const GLvoid *)plane_verticies);
    VAO_Attr(plane_VAO, plane_VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(plane_VAO, plane_VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(plane_VAO, plane_VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    // CUBE
    struct VAO cube_VAO = VAO_Create();
    struct VBO cube_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(cube_VBO, sizeof(cube_vertices), (const GLvoid *)cube_vertices);
    VAO_Attr(cube_VAO, cube_VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(cube_VAO, cube_VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(cube_VAO, cube_VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    // DEBUG
    struct VAO debug_VAO = VAO_Create();
    struct VBO debug_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(debug_VBO, sizeof(depth_debug_verticies), (const GLvoid *)depth_debug_verticies);
    VAO_Attr(debug_VAO, debug_VBO, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(debug_VAO, debug_VBO, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    sm.plane_VAO = plane_VAO.ID;
    sm.cube_VAO  = cube_VAO.ID;
    sm.debug_VAO = debug_VAO.ID;

    VAO_Unbind();
    VBO_Unbind();

    // LOAD TEXTURES
    struct Texture plane_tex = Texture_Create("../../Examples/res/textures/planks.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    struct Texture cube_tex  = Texture_Create("../../Examples/res/textures/woodne_box.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);

    // Shader_Uniform_Texture2D(shader_cube, "diffuseTexture", plane_tex);
    //  Shader_Uniform_Int(shader_cube, "shadowMap", 1);
    //   Shader_Uniform_Texture2D(shader_cube, "diffuseTexture", cube_tex);
    //   Shader_Uniform_Texture2D(shader_cube, "tex0", cube_tex);

    sm.plane_tex = plane_tex;
    sm.cube_tex  = cube_tex;

    // PLANE
    // Shader_Bind(sm.plane_shader);
    mat4 plane_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(plane_model, (vec3){0.0f, 0.0f, 0.0f});
    glm_mat4_copy(plane_model, sm.plane);

    // CUBES
    mat4 cube1_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(cube1_model, (vec3){0.0f, 1.5f, 0.0f});
    glm_scale(cube1_model, (vec3){0.5f, 0.5f, 0.5f});
    glm_mat4_copy(cube1_model, sm.cube1);

    mat4 cube2_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(cube2_model, (vec3){2.0f, 0.0f, 1.0f});
    glm_scale(cube2_model, (vec3){0.5f, 0.5f, 0.5f});
    glm_mat4_copy(cube2_model, sm.cube2);

    mat4 cube3_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(cube3_model, (vec3){-1.0f, 0.0f, 2.0f});
    glm_rotate(cube3_model, glm_rad(60.0f), (vec3){1.0f, 0.0f, 1.0f});
    glm_scale(cube3_model, (vec3){0.25f, 0.25f, 0.25f});
    glm_mat4_copy(cube3_model, sm.cube3);

    // CAMERA
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    // glm_vec3_copy((vec3){-39.353584f, 8.195490f, 30.373175f}, cam.position);
    // glm_vec3_copy((vec3){0.810961f, -0.213032f, -0.544940f}, cam.orientation);
    // cam.pitch = -12.300069f;
    // cam.yaw   = -33.899826f;
    sm.cam = cam;

    // LIGHTING
    vec3 light_position   = {-2.0f, 4.0f, -1.0f};
    mat4 light_orthogonal = GLM_MAT4_IDENTITY_INIT;
    mat4 light_view       = GLM_MAT4_IDENTITY_INIT;
    mat4 light_matrix     = GLM_MAT4_IDENTITY_INIT;

    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f, light_orthogonal);
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, light_view);
    glm_mat4_mul(light_orthogonal, light_view, light_matrix);

    Shader_Bind(shader_cube);
    Shader_Uniform_Mat4(shader_cube, "lightMatrix", light_matrix);
    Shader_Uniform_Vec3(shader_cube, "lightPos", light_position);
    Shader_Uniform_Int(shader_cube, "diffuseTexture", 0);
    Shader_Uniform_Int(shader_cube, "shadowMap", 1);

    // glm_mat4_copy(light_projection, sm.light_projection);

    struct Shadowmap shadowmap = Shadowmap_Create(shader_shadowmap);
    sm.shadowmap               = shadowmap;

    Shader_Bind(shader_shadowmap);
    Shader_Uniform_Mat4(shader_shadowmap, "lightMatrix", light_matrix);

    // DEBUG DEPTH
    Shader_Bind(shader_depth_debug);
    Shader_Uniform_Int(shader_depth_debug, "depthMap", 0);
}

static void Do_Shadowmapping_Rendering()
{
    //// SHADOW MAPPING -----------------------------------------------------------
    //// glEnable(GL_DEPTH_TEST);
    //// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //// glEnable(GL_DEPTH_TEST);
    //// 1. render depth of scene to texture (from light's perspective)
    //// --------------------------------------------------------------

    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Shader_Bind(sm.shader_shadowmap);
    // Shader_Uniform_Mat4(sm.shader_shadowmap, "lightProjection", sm.light_projection);

    //// Render the Scene for the cameras perspective
    // glViewport(0, 0, sm.shadowmap.width, sm.shadowmap.height);
    // glBindFramebuffer(GL_FRAMEBUFFER, sm.shadowmap.FBO_Id);
    // glClear(GL_DEPTH_BUFFER_BIT);

    // Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.plane);
    // Draw_Plane(sm.plane_VAO);

    // Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube1);
    // Draw_Cube(sm.cube_VAO);
    // Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube2);
    // Draw_Cube(sm.cube_VAO);
    // Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube3);
    // Draw_Cube(sm.cube_VAO);

    //// reset
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glViewport(0, 0, window.width, window.heigh);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_DEPTH_TEST);
    ////    SHADOW MAPPING -----------------------------------------------------------
}

void ShadowMapCubes_Update()
{
    Camera_Inputs(&sm.cam);

    // Render the Scene for the cameras perspective
    glViewport(0, 0, sm.shadowmap.width, sm.shadowmap.height);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.shadowmap.FBO_Id);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw scene for light POV to make shadowmap
    Shader_Bind(sm.shader_shadowmap);

    Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.plane);
    Draw_Plane(sm.plane_VAO);

    Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube1);
    Draw_Cube(sm.cube_VAO);
    Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube2);
    Draw_Cube(sm.cube_VAO);
    Shader_Uniform_Mat4(sm.shader_shadowmap, "model", sm.cube3);
    Draw_Cube(sm.cube_VAO);

    // reset
    glBindFramebuffer(GL_FRAMEBUFFER, 0);         // Switch back to the default framebuffer
    glViewport(0, 0, window.width, window.heigh); // Switch back to the default viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Scene normally now ------ init
    // Shader_Bind(sm.cube_shader);

    // Shader_Uniform_Vec3(sm.cube_shader, "viewPos", sm.cam.position);
    // Camera_View_Projection_To_Shader(sm.cam, sm.cube_shader, "camMatrix");

    // Texture_Bind(sm.plane_tex);
    // glActiveTexture(GL_TEXTURE0 + 0);
    // glBindTexture(GL_TEXTURE_2D, sm.plane_tex.ID);
    // glActiveTexture(GL_TEXTURE0 + 1);
    // glBindTexture(GL_TEXTURE_2D, sm.shadowmap.tex_Id);

    // Shader_Uniform_Mat4(sm.cube_shader, "model", sm.plane);
    // Draw_Plane(sm.plane_VAO);

    // glActiveTexture(GL_TEXTURE0 + 0);
    // glBindTexture(GL_TEXTURE_2D, sm.cube_tex.ID);

    // Shader_Uniform_Mat4(sm.cube_shader, "model", sm.cube1);
    // Draw_Cube(sm.cube_VAO);
    // Shader_Uniform_Mat4(sm.cube_shader, "model", sm.cube2);
    // Draw_Cube(sm.cube_VAO);
    // Shader_Uniform_Mat4(sm.cube_shader, "model", sm.cube3);
    // Draw_Cube(sm.cube_VAO);

    // DEBUG DEPTH (Comment out all code after init)
    Shader_Bind(sm.shader_depth_debug);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sm.shadowmap.tex_Id);
    Render_Depth_Debug_Map(sm.debug_VAO);
}

void ShadowMapCubes_OnExit()
{
}
