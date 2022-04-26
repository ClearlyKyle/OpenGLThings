#include "Ortho.h"

static struct OrthoDrawing
{
    VAO_t VAO;

    struct Shader shader;
} od;

void Ortho_Init()
{
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

    struct Shader shader_ortho = Shader_Create("../../Examples/30 - Orthographic Drawing/orthoshader.vs",
                                               "../../Examples/30 - Orthographic Drawing/orthoshader.fs",
                                               1,
                                               (struct VertexAttribute[]){
                                                   {.index = 0, .name = "position"}});

    od.shader = shader_ortho;

    // CUBE
    struct VAO cube_VAO = VAO_Create();
    struct VBO cube_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(cube_VBO, sizeof(cube_vertices), (const GLvoid *)cube_vertices);
    VAO_Attr(cube_VAO, cube_VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(cube_VAO, cube_VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(cube_VAO, cube_VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    od.VAO = cube_VAO;

    VAO_Unbind();
    VBO_Unbind();

    // Matricies Setup
    mat4 view = GLM_MAT4_IDENTITY_INIT;

    // loaction, at - center of the scene, up - up direction
    glm_lookat((vec3){-2.0f, 4.0f, -1.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f, projection);

    // Cube model matrix
    mat4 cube1_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(cube1_model, (vec3){0.0f, 0.0f, 3.0f});
    glm_scale(cube1_model, (vec3){0.5f, 0.5f, 0.5f});

    mat4 space_matrix;
    glm_mat4_mul(projection, view, space_matrix);

    Shader_Bind(od.shader);
    Shader_Uniform_Mat4(od.shader, "model", cube1_model);
    Shader_Uniform_Mat4(od.shader, "view", view);
    Shader_Uniform_Mat4(od.shader, "projection", projection);
    Shader_Uniform_Mat4(od.shader, "spaceMatrix", space_matrix);
}

void Ortho_Update()
{
    Shader_Bind(od.shader);
    // Draw Cube
    glBindVertexArray(od.VAO.ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0); // Unbind
}

void Ortho_OnExit()
{
    Shader_Destroy(od.shader);
    VAO_Destroy(od.VAO);
}
