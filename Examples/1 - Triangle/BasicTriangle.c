#include "BasicTriangle.h"

static struct BasicTriangle bt;

void BasicTriangle_Init()
{
    // Vertices coordinates
    const GLfloat vertices[] =
        {
            -0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,    // Lower left corner
            0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,     // Lower right corner
            0.0f, 0.5f * (float)sqrt(3) * 2 / 3, 0.0f,  // Upper corner
            -0.5f / 2, 0.5f * (float)sqrt(3) / 6, 0.0f, // Inner left
            0.5f / 2, 0.5f * (float)sqrt(3) / 6, 0.0f,  // Inner right
            0.0f, -0.5f * (float)sqrt(3) / 3, 0.0f      // Inner down
        };

    // Indices for vertices order
    GLuint indices[] =
        {
            0, 3, 5, // Lower left triangle
            3, 2, 4, // Lower right triangle
            5, 4, 1  // Upper triangle
        };

    // Generates Shader object using shaders defualt.vert and default.frag
    struct Shader triangle_shader = Shader_Create(
        "../../Examples/1 - Triangle/basic_triangle.vs",
        "../../Examples/1 - Triangle/basic_triangle.fs",
        1,
        (struct VertexAttribute[]){{.name = "aPos", .index = 0}});

    bt.shader = triangle_shader;

    // Generates Vertex Array Object and binds it
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    bt.vao = vao;

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (void *)indices);

    // Links VBO to VAO
    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    // VAO_Unbind();
    // VBO_Unbind();
    // EBO_Unbind();
}

void BasicTriangle_Update()
{
    Shader_Bind(bt.shader);
    VAO_Bind(bt.vao);

    // mode - what kind of primitives
    // count - number of elements to render
    // type - typeof(indicies) - unsinged byte, short, int
    // indicies - offset of the first index of the array currently bound
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
    // GL_UNSIGNED_INT - as we are drawing with "indicies"

    // VAO_Unbind();
}

void BasicTriangle_OnExit()
{
    Shader_Destroy(&bt.shader);
    VAO_Destroy(bt.vao);
}