
#include "TextureExample.h"

static struct TextureExample texture_example;

void TextureExample_Init()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {
            //     COORDINATES  /     COLORS      / TexCoord  //
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Lower left corner
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Upper left corner
            0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // Upper right corner
            0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f   // Lower right corner
        };

    // Indices for vertices order
    GLuint indices[] =
        {
            0, 2, 1, // Upper triangle
            0, 3, 2  // Lower triangle
        };

    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shader = Shader_Create(
        "../../Examples/3 - Textures/texture_example.vs",
        "../../Examples/3 - Textures/texture_example.fs",
        2, (struct VertexAttribute[]){{.index = 0, .name = "aPos"}, {.index = 1, .name = "aColor"}});

    texture_example.shader = shader;

    // Generates Vertex Array Object and binds it
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    texture_example.vao = vao;

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    // Generates Element Buffer Object and links it to indices
    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (const GLvoid *)indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)0);
    VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(vao, vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    // Texture
    const char *file_path = "../../Examples/res/textures/stone.png";
    struct Texture tex    = Texture_Create(file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    texture_example.tex   = tex;

    Shader_Uniform_Texture2D(shader, "tex0", tex);

    // Unbind all to prevent accidentally modifying them
    // VAO_Unbind();
    // VBO_Unbind();
    // EBO_Unbind();
}
void TextureExample_Update()
{
    // Gets ID of uniform called "scale"

    Shader_Bind(texture_example.shader);

    // Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
    Shader_Uniform_Float(texture_example.shader, "scale", 0.5f);
    // OR
    // glUniform1f(glGetUniformLocation(texture_example.shader.shader_id, "scale"), 0.5f);

    // Binds texture so that is appears in rendering
    Texture_Bind(texture_example.tex);

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(texture_example.vao);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TextureExample_OnExit()
{
    VAO_Destroy(texture_example.vao);
    Texture_Delete(texture_example.tex);
    Shader_Destroy(texture_example.shader);
}