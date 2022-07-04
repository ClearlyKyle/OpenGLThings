#include "SpecularMaps.h"

struct SpecularMaps
{
    struct Shader  shader[2];
    struct VAO     vao[2];
    struct Texture tex[2];
    struct Camera  cam;

    VAO_t cube_vao;

    GLsizei numer_of_indicies[2];
};

static struct SpecularMaps spec_maps;

void SpecularMaps_Init()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {//     COORDINATES     /        COLORS        /    TexCoord    /       NORMALS     //
         -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    // Indices for vertices order
    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3};

    GLfloat lightVertices[] =
        {//     COORDINATES     //
         -0.1f, -0.1f, 0.1f,
         -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, 0.1f,
         -0.1f, 0.1f, 0.1f,
         -0.1f, 0.1f, -0.1f,
         0.1f, 0.1f, -0.1f,
         0.1f, 0.1f, 0.1f};

    GLuint lightIndices[] =
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

    spec_maps.numer_of_indicies[0] = sizeof(indices) / sizeof(GLuint);
    spec_maps.numer_of_indicies[1] = sizeof(lightIndices) / sizeof(GLuint);

    {
        // Generates Shader object using shaders default.vert and default.frag
        struct Shader shader = Shader_Create(
            "../../Examples/7 - Specular Maps/default.vs",
            "../../Examples/7 - Specular Maps/default.fs",
            4,
            (struct VertexAttribute[]){
                {.index = 0, .name = "aPos"},
                {.index = 1, .name = "aColor"},
                {.index = 2, .name = "aTex"},
                {.index = 3, .name = "aNormal"}});

        spec_maps.shader[0] = shader;

        // Generates Vertex Array Object and binds it
        struct VAO vao = VAO_Create();
        VAO_Bind(vao);
        spec_maps.vao[0] = vao;

        // Generates Vertex Buffer Object and links it to vertices
        struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

        struct EBO ebo = EBO_Create();
        EBO_Buffer(ebo, sizeof(indices), (void *)indices);

        // Links VBO attributes such as coordinates and colors to VAO
        VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(0));
        VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
        VAO_Attr(vao, vbo, 2, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
        VAO_Attr(vao, vbo, 3, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(8 * sizeof(GLfloat)));

        // Unbind all to prevent accidentally modifying them
        VAO_Unbind();
        VBO_Unbind();
        // EBO_Unbind();
    }
    {
        // Generates Shader object using shaders default.vert and default.frag
        struct Shader shader = Shader_Create(
            "../../Examples/7 - Specular Maps/lights.vs",
            "../../Examples/7 - Specular Maps/lights.fs",
            1,
            (struct VertexAttribute[]){
                {.index = 0, .name = "aPos"},
            });

        spec_maps.shader[1] = shader;

        VAO_t cube         = Cube_Generate();
        spec_maps.cube_vao = cube;

        // Generates Vertex Array Object and binds it
        struct VAO vao   = VAO_Create();
        spec_maps.vao[1] = vao;

        // Generates Vertex Buffer Object and links it to vertices
        struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(vbo, sizeof(lightVertices), (const GLvoid *)lightVertices);

        struct EBO ebo = EBO_Create();
        EBO_Buffer(ebo, sizeof(lightIndices), (void *)lightIndices);

        // Links VBO attributes such as coordinates and colors to VAO
        VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

        // Unbind all to prevent accidentally modifying them
        VAO_Unbind();
        VBO_Unbind();
        // EBO_Unbind();
    }

    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};
    mat4 light_model    = GLM_MAT4_ZERO_INIT;
    glm_translate_make(light_model, light_position);

    vec3 pyramid_position = {0.0f, 0.0f, 0.0f};
    mat4 pyramid_model    = GLM_MAT4_ZERO_INIT;
    glm_translate_make(pyramid_model, pyramid_position);

    Shader_Bind(spec_maps.shader[0]);
    Shader_Uniform_Mat4(spec_maps.shader[0], "model", pyramid_model);
    Shader_Uniform_Vec4(spec_maps.shader[0], "lightColor", light_colour);
    Shader_Uniform_Vec3(spec_maps.shader[0], "lightPos", light_position);

    Shader_Bind(spec_maps.shader[1]);
    Shader_Uniform_Mat4(spec_maps.shader[1], "model", light_model);
    Shader_Uniform_Vec4(spec_maps.shader[1], "lightColor", light_colour);

    //  Texture
    const char    *file_path1 = "../../Examples/res/textures/planks.png";
    struct Texture tex1       = Texture_Create(file_path1, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    spec_maps.tex[0]          = tex1;

    const char    *file_path2 = "../../Examples/res/textures/planksSpec.png";
    struct Texture tex2       = Texture_Create(file_path2, GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    spec_maps.tex[1]          = tex2;

    Shader_Bind(spec_maps.shader[0]);
    Shader_Uniform_Texture2D(spec_maps.shader[0], "tex0", tex1);
    Shader_Uniform_Texture2D(spec_maps.shader[0], "tex1", tex2);

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){-1.880f, 0.509f, -1.031f}, 45.0f, 0.1f, 100.0f);
    glm_vec3_copy((vec3){-1.427915f, 0.640115f, -0.956351f}, cam.position);
    glm_vec3_copy((vec3){0.820859f, -0.247010f, 0.514954f}, cam.orientation);
    cam.pitch = -14.300634f;
    cam.yaw   = 32.101498f;

    // glm_vec3_copy((vec3){-39.353584f, 8.195490f, 30.373175f}, cam.position);
    // glm_vec3_copy((vec3){0.810961f, -0.213032f, -0.544940f}, cam.orientation);
    // cam.pitch = -12.300069f;
    // cam.yaw   = -33.899826f;

    spec_maps.cam = cam;
}

void SpecularMaps_Update()
{
    // Handles camera inputs
    Camera_Inputs(&spec_maps.cam);

    // SHADER 1

    // Tells OpenGL which Shader Program we want to use
    Shader_Bind(spec_maps.shader[0]);

    // Exports the camera Position to the Fragment Shader for specular lighting
    Shader_Uniform_Vec3(spec_maps.shader[0], "camPos", spec_maps.cam.position);

    // Export the camMatrix to the Vertex Shader of the pyramid
    Camera_View_Projection_To_Shader(spec_maps.cam, spec_maps.shader[0], "camMatrix");

    // Binds texture so that is appears in rendering
    Texture_Bind(spec_maps.tex[0]);
    Texture_Bind(spec_maps.tex[1]);

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(spec_maps.vao[0]);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, spec_maps.numer_of_indicies[0], GL_UNSIGNED_INT, 0);

    // SHADER 2

    // Tells OpenGL which Shader Program we want to use
    Shader_Bind(spec_maps.shader[1]);

    // Export the camMatrix to the Vertex Shader of the light cube
    Camera_View_Projection_To_Shader(spec_maps.cam, spec_maps.shader[1], "camMatrix");

    // Cube_Draw(spec_maps.cube_vao);

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(spec_maps.vao[1]);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, spec_maps.numer_of_indicies[1], GL_UNSIGNED_INT, 0);

    // glm_vec3_print(spec_maps.cam.orientation, stdout);
    // glm_vec3_print(spec_maps.cam.position, stdout);
}

void SpecularMaps_OnExit()
{
    Camera_Print_Values(spec_maps.cam);

    VAO_Destroy(spec_maps.cube_vao);
    VAO_Destroy(spec_maps.vao[0]);
    VAO_Destroy(spec_maps.vao[1]);
    Shader_Destroy(&spec_maps.shader[0]);
    Shader_Destroy(&spec_maps.shader[1]);
    Texture_Delete(spec_maps.tex[0]);
    Texture_Delete(spec_maps.tex[1]);
}
