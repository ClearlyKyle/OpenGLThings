#include "Shapes.h"

VAO_t Cube_Generate()
{
    const GLfloat vertices[] = {
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
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
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

    VAO_t VAO = VAO_Create();
    VBO_t VBO = VBO_Create(GL_ARRAY_BUFFER);

    VBO_Buffer(VBO, sizeof(vertices), (const GLvoid *)vertices);

    VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));                   // POSITION
    VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat))); // NORMALS
    VAO_Attr(VAO, VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat))); // TEXTURE

    return VAO;
}

VAO_t Quad_Generate()
{
    const GLfloat vertices[] =
        {// COORDINATES    // NORMALS        // TEX
         -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    // Indices for vertices order
    const GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3};

    VAO_t VAO = VAO_Create();
    VBO_t VBO = VBO_Create(GL_ARRAY_BUFFER);

    EBO_t EBO = EBO_Create();
    EBO_Buffer(EBO, sizeof(indices), (const GLvoid *)indices);

    VBO_Buffer(VBO, sizeof(vertices), (const GLvoid *)vertices);

    VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));                   // POSITION
    VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat))); // NORMALS
    VAO_Attr(VAO, VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat))); // TEXTURE

    return VAO;
};

VAO_t Sphere_Generate()
{
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;

    const size_t size = (X_SEGMENTS + 1) * (Y_SEGMENTS + 1) * 3;

    GLfloat      *positions  = (GLfloat *)malloc(sizeof(GLfloat) * size);
    GLfloat      *normals    = (GLfloat *)malloc(sizeof(GLfloat) * size);
    GLfloat      *tex_coords = (GLfloat *)malloc(sizeof(GLfloat) * (Y_SEGMENTS + 1) * (X_SEGMENTS + 1) * 2);
    unsigned int *indices    = (unsigned int *)malloc(sizeof(unsigned int) * Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    unsigned int k         = 0;
    unsigned int tex_index = 0;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            const float xSegment = (float)x / (float)X_SEGMENTS;
            const float ySegment = (float)y / (float)Y_SEGMENTS;
            const float xPos     = cosf(xSegment * 2.0f * (float)M_PI) * sinf(ySegment * (float)M_PI);
            const float yPos     = cosf(ySegment * (float)M_PI);
            const float zPos     = sinf(xSegment * 2.0f * (float)M_PI) * sinf(ySegment * (float)M_PI);

            positions[k + 0] = xPos;
            positions[k + 1] = yPos;
            positions[k + 2] = zPos;

            normals[k + 0] = xPos;
            normals[k + 1] = yPos;
            normals[k + 2] = zPos;
            k += 3;

            tex_coords[tex_index++] = xSegment;
            tex_coords[tex_index++] = ySegment;
            // uv.push_back(glm::vec2(xSegment, ySegment));
        }
    }

    bool         oddRow = false;
    unsigned int index  = 0;
    for (unsigned int y = 0; y < Y_SEGMENTS; y++)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; x++)
            {
                indices[index++] = y * (X_SEGMENTS + 1) + x;
                indices[index++] = (y + 1) * (X_SEGMENTS + 1) + x;
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; x--)
            {
                indices[index++] = (y + 1) * (X_SEGMENTS + 1) + x;
                indices[index++] = y * (X_SEGMENTS + 1) + x;
            }
        }
        oddRow = !oddRow;
    }

    VAO_t sphere = VAO_Create();

    EBO_t indi = EBO_Create();
    EBO_Buffer(indi, sizeof(GLfloat) * X_SEGMENTS * (Y_SEGMENTS + 1) * 2, (const GLvoid *)indices);

    VBO_t verts = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(verts, sizeof(GLfloat) * size, (const GLvoid *)positions);
    VBO_t norm = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(norm, sizeof(GLfloat) * size, (const GLvoid *)normals);
    VBO_t tex = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(tex, sizeof(GLfloat) * Y_SEGMENTS * (X_SEGMENTS + 1) * 2, (const GLvoid *)tex_coords);

    VAO_Attr(sphere, verts, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(sphere, norm, 1, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(sphere, tex, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)(0));

    free(positions);
    free(normals);
    free(indices);
    free(tex_coords);

    return sphere;
}