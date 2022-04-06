#include "UBOExample.h"

static struct UBOExample uboexample;

void UBOExample_Init()
{
    GLfloat cubeVertices[] = {
        // positions
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,

        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,

        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,

        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,

        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,

        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
    };

    uboexample.shader_red = Shader_Create("../../Examples/shaders/20/vertex_shader.vs", "../../Examples/shaders/20/red.fs",
                                          1, (struct VertexAttribute[]){{.name = "aPos", .index = 0}});
    uboexample.shader_green = Shader_Create("../../Examples/shaders/20/vertex_shader.vs", "../../Examples/shaders/20/green.fs",
                                            1, (struct VertexAttribute[]){{.name = "aPos", .index = 0}});
    uboexample.shader_blue = Shader_Create("../../Examples/shaders/20/vertex_shader.vs", "../../Examples/shaders/20/blue.fs",
                                           1, (struct VertexAttribute[]){{.name = "aPos", .index = 0}});
    uboexample.shader_yellow = Shader_Create("../../Examples/shaders/20/vertex_shader.vs", "../../Examples/shaders/20/yellow.fs",
                                             1, (struct VertexAttribute[]){{.name = "aPos", .index = 0}});

    struct VAO vao = VAO_Create();
    uboexample.vao = vao;

    struct VBO cube_vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(cube_vbo, sizeof(cubeVertices), (const GLvoid *)cubeVertices);

    VAO_Attr(vao, cube_vbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    // Configure Uniform Buffer Object
    UBO_Link_Shader(uboexample.shader_red, 0, "Matrices");
    UBO_Link_Shader(uboexample.shader_green, 0, "Matrices");
    UBO_Link_Shader(uboexample.shader_blue, 0, "Matrices");
    UBO_Link_Shader(uboexample.shader_yellow, 0, "Matrices");

    // Create the UBO Buffer
    struct UBO ubo = UBO_Create(0, 0, 2 * sizeof(mat4));
    uboexample.ubo = ubo;

    // store the projection matrix (we only do this once now)
    mat4 projection = GLM_MAT4_ZERO_INIT;
    glm_perspective(45.0f, window.aspect_ratio, 0.1f, 100.0f, projection);
    UBO_Data(ubo, 0, sizeof(mat4), projection);

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    uboexample.cam = cam;
}

void UBOExample_Update()
{
    Camera_Inputs(&uboexample.cam);

    mat4 view_matrix;
    Camera_Get_View_Matrix(uboexample.cam, view_matrix);

    UBO_Data(uboexample.ubo, sizeof(mat4), sizeof(mat4), view_matrix);

    // DRAW 4 CUBES
    VAO_Bind(uboexample.vao);
    mat4 model;

    // RED
    Shader_Bind(uboexample.shader_red);
    glm_translate_make(model, (vec3){-0.75f, 0.75f, 0.0f});
    Shader_Uniform_Mat4(uboexample.shader_red, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // GREEN
    Shader_Bind(uboexample.shader_green);
    glm_translate_make(model, (vec3){0.75f, 0.75f, 0.0f});
    Shader_Uniform_Mat4(uboexample.shader_green, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // YELLOW
    Shader_Bind(uboexample.shader_yellow);
    glm_translate_make(model, (vec3){-0.75f, -0.75f, 0.0f});
    Shader_Uniform_Mat4(uboexample.shader_yellow, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // BLUE
    Shader_Bind(uboexample.shader_blue);
    glm_translate_make(model, (vec3){0.75f, -0.75f, 0.0f});
    Shader_Uniform_Mat4(uboexample.shader_blue, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void UBOExample_OnExit()
{
    VAO_Destroy(uboexample.vao);
    Shader_Destroy(uboexample.shader_red);
    Shader_Destroy(uboexample.shader_blue);
    Shader_Destroy(uboexample.shader_green);
    Shader_Destroy(uboexample.shader_yellow);
}
