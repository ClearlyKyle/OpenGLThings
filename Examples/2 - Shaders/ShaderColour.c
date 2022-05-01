#include "ShaderColour.h"

static struct ShaderColour sc;

void ShaderColour_Init()
{
    // Vertices coordinates
    const GLfloat vertices[] =
        {
            //               COORDINATES                /COLORS
            -0.5f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f, 0.8f, 0.3f, 0.02f,  // Lower left corner
            0.5f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f, 0.8f, 0.3f, 0.02f,   // Lower right corner
            0.0f, 0.5f * (float)sqrt(3) * 2 / 3, 0.0f, 1.0f, 0.6f, 0.32f,    // Upper corner
            -0.25f, 0.5f * (float)sqrt(3) * 1 / 6, 0.0f, 0.9f, 0.45f, 0.17f, // Inner left
            0.25f, 0.5f * (float)sqrt(3) * 1 / 6, 0.0f, 0.9f, 0.45f, 0.17f,  // Inner right
            0.0f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f, 0.8f, 0.3f, 0.02f    // Inner down
        };

    // Indices for vertices order
    const GLuint indices[] =
        {
            0, 3, 5, // Lower left triangle
            3, 2, 4, // Lower right triangle
            5, 4, 1  // Upper triangle
        };

    // Generates Shader object using shaders defualt.vert and default.frag
    struct Shader shader = Shader_Create(
        "../../Examples/shaders/2/coloured_triangle.vs",
        "../../Examples/shaders/2/coloured_triangle.fs",
        2, (struct VertexAttribute[]){{.index = 0, .name = "aPos"}, {.index = 1, .name = "aColor"}});

    sc.shader = shader;

    // Generates Vertex Array Object and binds it
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    sc.vao = vao;

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (const GLvoid *)indices);

    // Links VBO to VAO
    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)0);
    VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    // Unbind all to prevent accidentally modifying them
    // VAO_Unbind();
    // VBO_Unbind();
    // EBO_Unbind();
}

static float increment = 0.00001f;

void ShaderColour_Update()
{
    Shader_Bind(sc.shader);

    Shader_Uniform_Float(sc.shader, "scale", increment);
    increment += 0.00001f;

    VAO_Bind(sc.vao);

    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
}

void ShaderColour_OnExit()
{
    VAO_Destroy(sc.vao);
    Shader_Destroy(&sc.shader);
}
