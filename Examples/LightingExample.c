
#include "LightingExample.h"

static struct LightingExample lights;

void LightingExample_Init()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {
            //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
            -0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // Bottom side
            -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 5.0f, 0.0f, -1.0f, 0.0f, // Bottom side
            0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 5.0f, 0.0f, -1.0f, 0.0f,  // Bottom side
            0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f,   // Bottom side

            -0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f, -0.8f, 0.5f, 0.0f,  // Left Side
            -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f, -0.8f, 0.5f, 0.0f, // Left Side
            0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f, -0.8f, 0.5f, 0.0f,   // Left Side

            -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f, 0.0f, 0.5f, -0.8f, // Non-facing side
            0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f, 0.0f, 0.5f, -0.8f,  // Non-facing side
            0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f, 0.0f, 0.5f, -0.8f,   // Non-facing side

            0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f, 0.8f, 0.5f, 0.0f, // Right side
            0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f, 0.8f, 0.5f, 0.0f,  // Right side
            0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f, 0.8f, 0.5f, 0.0f,  // Right side

            0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f, 0.0f, 0.5f, 0.8f,  // Facing side
            -0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f, 0.0f, 0.5f, 0.8f, // Facing side
            0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f, 0.0f, 0.5f, 0.8f   // Facing side
        };
    // Indices for vertices order
    GLuint indices[] =
        {
            0, 1, 2,    // Bottom side
            0, 2, 3,    // Bottom side
            4, 6, 5,    // Left side
            7, 9, 8,    // Non-facing side
            10, 12, 11, // Right side
            13, 15, 14  // Facing side
        };

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

    /* PYRAMID SAHDER ---------------------------------------------------------------------*/
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shader1 = Shader_Create(
        "../../shaders/default.vs",
        "../../shaders/default.fs",
        4,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aColor"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aNormal"}});

    lights.shader[0] = shader1;

    // Generates Vertex Array Object and binds it
    struct VAO vao1 = VAO_Create();
    VAO_Bind(vao1);

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (void *)indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(vao1, vbo, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(vao1, vbo, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(vao1, vbo, 2, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
    VAO_Attr(vao1, vbo, 3, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(8 * sizeof(GLfloat)));

    lights.vao[0] = vao1;
    // Unbind all to prevent accidentally modifying them
    VBO_Unbind();
    VAO_Unbind();
    // EBO_Unbind();
    /* PYRAMID SAHDER ---------------------------------------------------------------------*/

    /* LIGHT SAHDER ---------------------------------------------------------------------*/
    // Shader for light cube
    struct Shader shader2 = Shader_Create(
        "../../shaders/lights.vs",
        "../../shaders/lights.fs",
        1,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"}});

    lights.shader[1] = shader2;

    // Generates Vertex Array Object and binds it
    struct VAO vao2 = VAO_Create();
    VAO_Bind(vao2);

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo2 = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo2, sizeof(lightVertices), (const GLvoid *)lightVertices);

    struct EBO ebo2 = EBO_Create();
    EBO_Buffer(ebo2, sizeof(lightIndices), (void *)lightIndices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(vao2, vbo2, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    lights.vao[1] = vao2;

    // Unbind all to prevent accidentally modifying them
    VBO_Unbind();
    VAO_Unbind();
    // EBO_Unbind();
    /* LIGHT SAHDER ---------------------------------------------------------------------*/

    vec4 light_colour = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};
    mat4 light_model;
    glm_translate_make(light_model, light_position);

    vec3 pyramid_position = {0.0f, 0.0f, 0.0f};
    mat4 pyramid_model;
    glm_translate_make(pyramid_model, pyramid_position);

    Shader_Bind(lights.shader[1]);
    Shader_Uniform_Mat4(lights.shader[1], "model", light_model);
    Shader_Uniform_Vec4(lights.shader[1], "lightColor", light_colour);

    Shader_Bind(lights.shader[0]);
    Shader_Uniform_Mat4(lights.shader[0], "model", pyramid_model);
    Shader_Uniform_Vec4(lights.shader[0], "lightColor", light_colour);
    // ALWATS CHECK WHAT VEC SIZE SENDING TO UNIFORM, VEC3? VEC4?
    Shader_Uniform_Vec3(lights.shader[0], "lightPos", light_position);

    // Texture
    const char *file_path = "../../Examples/Textures/brick.png";
    struct Texture tex = Texture_Create(file_path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    lights.tex = tex;

    Texture_Uniform(lights.shader[0], "tex0", 0);

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    lights.cam = cam;
}

void LightingExample_Update()
{
    // Handles camera inputs
    Camera_Inputs(&lights.cam);

    // Tells OpenGL which Shader Program we want to use
    Shader_Bind(lights.shader[0]);

    // Exports the camera Position to the Fragment Shader for specular lighting
    Shader_Uniform_Vec3(lights.shader[0], "camPos", lights.cam.position);

    // Export the camMatrix to the Vertex Shader of the pyramid
    Camera_View_Projection_To_Shader(lights.cam, lights.shader[0], "camMatrix");

    // Binds texture so that is appears in rendering
    Texture_Bind(lights.tex);

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(lights.vao[0]);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

    // Tells OpenGL which Shader Program we want to use
    Shader_Bind(lights.shader[1]);

    // Export the camMatrix to the Vertex Shader of the light cube
    Camera_View_Projection_To_Shader(lights.cam, lights.shader[1], "camMatrix");

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(lights.vao[1]);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void LightingExample_OnExit()
{
    VAO_Destroy(lights.vao[0]);
    VAO_Destroy(lights.vao[1]);
    Shader_Destroy(lights.shader[0]);
    Shader_Destroy(lights.shader[1]);
    Texture_Delete(lights.tex);
}
