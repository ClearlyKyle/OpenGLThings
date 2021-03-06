#include "Basic3D.h"

static struct Basic3D basic3d;

void Basic3D_Init()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {//     COORDINATES     /        COLORS      /   TexCoord  //
         -0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
         -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
         0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f};

    // Indices for vertices order
    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3,
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4};

    basic3d.numer_of_indicies = sizeof(indices) / sizeof(int);

    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shader = Shader_Create(
        "../../Examples/4 - Basic 3D/basic_3D.vs",
        "../../Examples/4 - Basic 3D/basic_3D.fs",
        2, (struct VertexAttribute[]){{.index = 0, .name = "aPos"}, {.index = 1, .name = "aColor"}});

    basic3d.shader = shader;

    // Generates Vertex Array Object and binds it
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    basic3d.vao = vao;

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    // Generates Element Buffer Object and links it to indices
    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (const GLvoid *)indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(vao, vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    // Texture
    const char    *file_path = "../../Examples/res/textures/brick.png";
    struct Texture tex       = Texture_Create(file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    basic3d.tex              = tex;

    Shader_Uniform_Texture2D(basic3d.shader, "tex0", tex);

    // VAO_Unbind();
    // VBO_Unbind();
    // EBO_Unbind();
}

static float rotation = 0.0f;

// Pass in window and time?
void Basic3D_Update()
{
    Shader_Bind(basic3d.shader);

    rotation += (float)(window.frame_time * 100.0);

    // Initializes matrices so they are not the null matrix
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 trans = {1.0f, 0.0f, -2.0f};
    vec3 rot   = {0.0f, 1.0f, 0.0f};
    vec4 qRot  = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 scale = {1.5f, 1.0f, 1.0f};

    // Do the Model Matrix calculation on the CPU
    // Transform the matrices to their correct form
    mat4 mat_trans;
    mat4 mat_rot;
    mat4 mat_scale;
    mat4 matrix = GLM_MAT4_ZERO_INIT;
    glm_translate_make(mat_trans, trans);
    glm_quat_mat4(qRot, mat_rot);
    glm_scale_make(mat_scale, scale);

    // trans * rot * sca
    glm_mat4_mul(mat_trans, mat_rot, matrix);
    glm_mat4_mul(matrix, mat_scale, matrix);

    Shader_Uniform_Mat4(basic3d.shader, "matrix", matrix);

    // Do the Model Matrix calculation on the GPU in the shader
    // Outputs the matrices into the Vertex Shader
    // Assigns different transformations to each matrix
    glm_translate_make(view, (vec3){0.0f, 0.0f, -2.0f});
    glm_rotate_make(model, glm_rad(rotation), trans);
    glm_perspective(glm_rad(45.0f), (float)window.heigh / window.width, 0.1f, 100.0f, proj);

    Shader_Uniform_Mat4(basic3d.shader, "model", model);
    Shader_Uniform_Mat4(basic3d.shader, "view", view);
    Shader_Uniform_Mat4(basic3d.shader, "proj", proj);

    // Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
    Shader_Uniform_Float(basic3d.shader, "scale", 0.2f);

    // Binds texture so that is appears in rendering
    Texture_Bind(basic3d.tex);

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(basic3d.vao);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, basic3d.numer_of_indicies, GL_UNSIGNED_INT, 0);
}

void Basic3D_OnExit()
{
    // Delete all the objects we've created
    VAO_Destroy(basic3d.vao);
    // VBO_Destroy(basic3d.vbo);
    // EBO_Destroy(basic3d.ebo);

    Shader_Destroy(&basic3d.shader);
    Texture_Delete(basic3d.tex);
}