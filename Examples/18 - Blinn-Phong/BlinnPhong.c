#include "BlinnPhong.h"

static struct BlinnPhong
{
    struct Camera cam;

    struct Shader surface_shader;
    struct Shader light_shader;

    struct Texture tex1;
    struct Texture tex2;

    struct VAO surface_VAO;
    struct VAO light_VAO;

    GLsizei surface_num_indicies;
    GLsizei light_num_indicies;

} bp;

void BlinnPhong_Init()
{
    // Vertices coordinates
    const GLfloat surface_verticies[] =
        {//     COORDINATES        /        COLORS         /    TexCoord       /       NORMALS      //
         -10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         -10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f,
         10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f,
         10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    // Indices for vertices order
    const GLuint surface_indicies[] =
        {
            0, 1, 2,
            0, 2, 3};

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
            0, 1, 2,
            0, 2, 3,
            0, 4, 7,
            0, 7, 3,
            3, 7, 6,
            3, 6, 2,
            2, 6, 5,
            2, 5, 1,
            1, 5, 4,
            1, 4, 0,
            4, 5, 6,
            4, 6, 7};

    bp.surface_num_indicies = sizeof(surface_indicies) / sizeof(GLuint);
    bp.light_num_indicies   = sizeof(light_indices) / sizeof(GLuint);

    //----------------------------------------------------------------
    // SURFACE -------------------------------------------------------
    struct Shader surface_shader = Shader_Create(
        "../../Examples/18 - Blinn-Phong/default.vs",
        "../../Examples/18 - Blinn-Phong/default.fs",
        4,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aColor"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aNormal"}});

    // Generates Vertex Array Object and binds it
    struct VAO surface_vao = VAO_Create();

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO surface_vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(surface_vbo, sizeof(surface_verticies), (const GLvoid *)surface_verticies);

    struct EBO surface_ebo = EBO_Create();
    EBO_Buffer(surface_ebo, sizeof(surface_indicies), (void *)surface_indicies);

    // Links surface_vbo attributes such as coordinates and colors to surface_vao
    VAO_Attr(surface_vao, surface_vbo, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(surface_vao, surface_vbo, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(surface_vao, surface_vbo, 2, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
    VAO_Attr(surface_vao, surface_vbo, 3, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(8 * sizeof(GLfloat)));

    // Unbind all to prevent accidentally modifying them
    VAO_Unbind();
    VBO_Unbind();

    bp.surface_shader = surface_shader;
    bp.surface_VAO    = surface_vao;
    // SURFACE -------------------------------------------------------

    //----------------------------------------------------------------

    // LIGHT ---------------------------------------------------------
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader light_shader = Shader_Create(
        "../../Examples/18 - Blinn-Phong/lights.vs",
        "../../Examples/18 - Blinn-Phong/lights.fs",
        1,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"}});

    // Generates Vertex Array Object and binds it
    struct VAO light_vao = VAO_Create();

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO light_abo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(light_abo, sizeof(light_vertices), (const GLvoid *)light_vertices);

    struct EBO light_ebo = EBO_Create();
    EBO_Buffer(light_ebo, sizeof(light_indices), (void *)light_indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(light_vao, light_abo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    // Unbind all to prevent accidentally modifying them
    VAO_Unbind();
    VBO_Unbind();

    bp.light_shader = light_shader;
    bp.light_VAO    = light_vao;

    // LIGHT ---------------------------------------------------------
    //----------------------------------------------------------------

    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};
    mat4 light_model    = GLM_MAT4_ZERO_INIT;
    glm_translate_make(light_model, light_position);

    vec3 object_position = {0.0f, 0.0f, 0.0f};
    mat4 object_model    = GLM_MAT4_ZERO_INIT;
    glm_translate_make(object_model, object_position);

    Shader_Bind(surface_shader);
    Shader_Uniform_Mat4(surface_shader, "model", object_model);
    Shader_Uniform_Vec4(surface_shader, "lightColor", light_colour);
    Shader_Uniform_Vec3(surface_shader, "lightPos", light_position);

    Shader_Bind(light_shader);
    Shader_Uniform_Mat4(light_shader, "model", light_model);
    Shader_Uniform_Vec4(light_shader, "lightColor", light_colour);

    // TEXTURES
    const char    *file_path1 = "../../Examples/res/textures/planks.png";
    struct Texture tex1       = Texture_Create(file_path1, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    bp.tex1                   = tex1;

    const char    *file_path2 = "../../Examples/res/textures/planksSpec.png";
    struct Texture tex2       = Texture_Create(file_path2, GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    bp.tex2                   = tex2;

    Shader_Uniform_Texture2D(surface_shader, "tex0", tex1);
    Shader_Uniform_Texture2D(surface_shader, "tex1", tex2);

    // CAMERA
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){-1.880f, 0.509f, -1.031f}, 45.0f, 0.1f, 100.0f);
    Camera_Set_Orientation(&cam, (vec3){0.85f, -0.17f, 0.48f});

    // glm_vec3_copy((vec3){-39.353584f, 8.195490f, 30.373175f}, cam.position);
    // glm_vec3_copy((vec3){0.810961f, -0.213032f, -0.544940f}, cam.orientation);
    // cam.pitch = -12.300069f;
    // cam.yaw   = -33.899826f;

    bp.cam = cam;
}

void BlinnPhong_Update()
{
    // Handles camera inputs
    Camera_Inputs(&bp.cam);

    // SURFACE SHADER
    Shader_Bind(bp.surface_shader);

    Shader_Uniform_Vec3(bp.surface_shader, "camPos", bp.cam.position);
    Camera_View_Projection_To_Shader(bp.cam, bp.surface_shader, "camMatrix");

    Texture_Bind(bp.tex1);
    Texture_Bind(bp.tex2);

    VAO_Bind(bp.surface_VAO);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, bp.surface_num_indicies, GL_UNSIGNED_INT, 0);

    // LIGHT SHADER
    Shader_Bind(bp.light_shader);

    Camera_View_Projection_To_Shader(bp.cam, bp.light_shader, "camMatrix");

    VAO_Bind(bp.light_VAO);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, bp.light_num_indicies, GL_UNSIGNED_INT, 0);
}

void BlinnPhong_OnExit()
{
    Camera_Print_Values(bp.cam);

    VAO_Destroy(bp.surface_VAO);
    VAO_Destroy(bp.light_VAO);

    Shader_Destroy(bp.surface_shader);
    Shader_Destroy(bp.light_shader);

    Texture_Delete(bp.tex1);
    Texture_Delete(bp.tex2);
}
