#include "Sphere_Rendering.h"

#include <math.h>

struct Sphere
{
    Camera_t cam;
    Shader_t shader;

    Mesh_t sphere_mesh;

    Texture_t tex;

    struct VAO   sphere;
    unsigned int index_count;
} sph;

// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector

#define EPSILON 0.000001f
static void computeFaceNormal(vec3 normal, float x1, float y1, float z1, // v1
                              float x2, float y2, float z2,              // v2
                              float x3, float y3, float z3)              // v3
{
    glm_vec3_zero(normal);
    float nx, ny, nz;

    // find 2 edge vectors: v1-v2, v1-v3
    const float ex1 = x2 - x1;
    const float ey1 = y2 - y1;
    const float ez1 = z2 - z1;
    const float ex2 = x3 - x1;
    const float ey2 = y3 - y1;
    const float ez2 = z3 - z1;

    // cross product: e1 x e2
    // [ex1, ey1, ez1] X [ex2, ey2, ez2]
    nx = ey1 * ez2 - ez1 * ey2;
    ny = ez1 * ex2 - ex1 * ez2;
    nz = ex1 * ey2 - ey1 * ex2;

    // normalize only if the length is > 0
    const float length = sqrtf(nx * nx + ny * ny + nz * nz);
    if (length > EPSILON)
    {
        // normalize
        const float lengthInv = 1.0f / length;
        normal[0]             = nx * lengthInv;
        normal[1]             = ny * lengthInv;
        normal[2]             = nz * lengthInv;
        return;
    }
    // normal[0] = 0.0f;
    // normal[1] = 0.0f;
    // normal[2] = 0.0f;
}

static VAO_t Sphere_Generate_Method_1(const float        radius,
                                      const unsigned int sectors,
                                      const unsigned int stacks,
                                      unsigned int      *index_count)
{
    // tmp vertex definition (x,y,z,s,t)
    struct Vertex
    {
        float x, y, z, s, t;
    };
    // struct Vertex
    //{
    //     vec3 pos;
    //     vec3 st;
    // };
    struct Vertex *tmpVertices = (struct Vertex *)malloc(sizeof(struct Vertex) * (stacks + 1) * (sectors + 1));

    const float sectorStep = (float)(2.0 * M_PI) / (float)sectors;
    const float stackStep  = (float)M_PI / (float)stacks;
    float       sectorAngle, stackAngle;

    // compute all vertices first, each vertex contains (x,y,z,s,t) except normal
    unsigned int index = 0;
    for (unsigned int i = 0; i <= stacks; i++)
    {
        stackAngle     = (float)(M_PI / 2.0) - i * stackStep; // starting from pi/2 to -pi/2
        const float xy = radius * cosf(stackAngle);           // r * cos(u)
        const float z  = radius * sinf(stackAngle);           // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (unsigned int j = 0; j <= sectors; j++)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            const struct Vertex tmp = {
                .x = xy * cosf(sectorAngle), // x = r * cos(u) * cos(v)
                .y = xy * sinf(sectorAngle), // y = r * cos(u) * sin(v)
                .z = z,                      // z = r * sin(u)
                .s = (float)j / sectors,     // s
                .t = (float)i / stacks,      // t
            };

            tmpVertices[index++] = tmp;
        }
    }

    // Vertex data
    const unsigned int size      = (stacks + 3) * (sectors + 4) * 3 * 3;
    GLfloat           *verticies = (GLfloat *)malloc(sizeof(GLfloat) * size);                               // POS
    GLfloat           *normals   = (GLfloat *)malloc(sizeof(GLfloat) * size);                               // NOR
    GLfloat           *texCoords = (GLfloat *)malloc(sizeof(GLfloat) * stacks * (sectors - 1) * 2 * 2 * 2); // TEX
    unsigned int      *indicies  = (unsigned int *)malloc(sizeof(unsigned int) * size);                     // IND

    struct Vertex v1, v2, v3, v4; // 4 vertex positions and tex coords
    vec3          n;              // 1 face normal

    int vi1, vi2;

    unsigned int vert_index = 0;
    unsigned int norm_index = 0;
    unsigned int tex_index  = 0;
    unsigned int indi_index = 0;
    index                   = 0;

    for (unsigned int i = 0; i < stacks; ++i)
    {
        vi1 = i * (sectors + 1); // in dex of tmpVertices
        vi2 = (i + 1) * (sectors + 1);

        for (unsigned int j = 0; j < sectors; ++j, ++vi1, ++vi2)
        {
            // get 4 vertices per sector
            //  v1--v3
            //  |    |
            //  v2--v4
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // if 1st stack and last stack, store only 1 triangle per sector
            // otherwise, store 2 triangles (quad) per sector
            if (i == 0) // a triangle for first stack ==========================
            {
                // put a triangle
                verticies[vert_index++] = v1.x;
                verticies[vert_index++] = v1.y;
                verticies[vert_index++] = v1.z;

                verticies[vert_index++] = v2.x;
                verticies[vert_index++] = v2.y;
                verticies[vert_index++] = v2.z;

                verticies[vert_index++] = v4.x;
                verticies[vert_index++] = v4.y;
                verticies[vert_index++] = v4.z;

                // put tex coords of triangle
                texCoords[tex_index++] = v1.s;
                texCoords[tex_index++] = v1.t;

                texCoords[tex_index++] = v2.s;
                texCoords[tex_index++] = v2.t;

                texCoords[tex_index++] = v4.s;
                texCoords[tex_index++] = v4.t;

                // put normal
                computeFaceNormal(n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v4.x, v4.y, v4.z);
                for (unsigned int k = 0; k < 3; ++k) // same normals for 3 vertices
                {
                    normals[norm_index++] = n[0];
                    normals[norm_index++] = n[1];
                    normals[norm_index++] = n[2];
                }

                // put indices of 1 triangle
                indicies[indi_index++] = index + 0;
                indicies[indi_index++] = index + 1;
                indicies[indi_index++] = index + 2;

                index += 3; // for next
            }
            else if (i == (stacks - 1)) // a triangle for last stack =========
            {
                // put a triangle
                verticies[vert_index++] = v1.x;
                verticies[vert_index++] = v1.y;
                verticies[vert_index++] = v1.z;

                verticies[vert_index++] = v2.x;
                verticies[vert_index++] = v2.y;
                verticies[vert_index++] = v2.z;

                verticies[vert_index++] = v3.x;
                verticies[vert_index++] = v3.y;
                verticies[vert_index++] = v3.z;

                // put tex coords of triangle
                texCoords[tex_index++] = v1.s;
                texCoords[tex_index++] = v1.t;

                texCoords[tex_index++] = v2.s;
                texCoords[tex_index++] = v2.t;

                texCoords[tex_index++] = v3.s;
                texCoords[tex_index++] = v3.t;

                // put normal
                computeFaceNormal(n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
                for (unsigned int k = 0; k < 3; ++k) // same normals for 3 vertices
                {
                    normals[norm_index++] = n[0];
                    normals[norm_index++] = n[1];
                    normals[norm_index++] = n[2];
                }

                // put indices of 1 triangle
                indicies[indi_index++] = index + 0;
                indicies[indi_index++] = index + 1;
                indicies[indi_index++] = index + 2;

                index += 3; // for next
            }
            else // 2 triangles for others ====================================
            {
                // put quad vertices: v1-v2-v3-v4
                verticies[vert_index++] = v1.x;
                verticies[vert_index++] = v1.y;
                verticies[vert_index++] = v1.z;

                verticies[vert_index++] = v2.x;
                verticies[vert_index++] = v2.y;
                verticies[vert_index++] = v2.z;

                verticies[vert_index++] = v3.x;
                verticies[vert_index++] = v3.y;
                verticies[vert_index++] = v3.z;

                verticies[vert_index++] = v4.x;
                verticies[vert_index++] = v4.y;
                verticies[vert_index++] = v4.z;

                // put tex coords of quad
                texCoords[tex_index++] = v1.s;
                texCoords[tex_index++] = v1.t;

                texCoords[tex_index++] = v2.s;
                texCoords[tex_index++] = v2.t;

                texCoords[tex_index++] = v3.s;
                texCoords[tex_index++] = v3.t;

                texCoords[tex_index++] = v4.s;
                texCoords[tex_index++] = v4.t;

                // put normal
                computeFaceNormal(n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
                for (unsigned int k = 0; k < 4; ++k) // same normals for 4 vertices
                {
                    normals[norm_index++] = n[0];
                    normals[norm_index++] = n[1];
                    normals[norm_index++] = n[2];
                }

                // put indices of quad (2 triangles)
                indicies[indi_index++] = index + 0;
                indicies[indi_index++] = index + 1;
                indicies[indi_index++] = index + 2;

                indicies[indi_index++] = index + 2;
                indicies[indi_index++] = index + 1;
                indicies[indi_index++] = index + 3;

                index += 4; // for next
            }
        }
    }

    *index_count = indi_index;

    VAO_t sphere = VAO_Create();

    EBO_t indi = EBO_Create();
    EBO_Buffer(indi, sizeof(GLfloat) * size, (const GLvoid *)indicies);

    VBO_t verts = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(verts, sizeof(GLfloat) * size, (const GLvoid *)verticies);
    VBO_t norm = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(norm, sizeof(GLfloat) * size, (const GLvoid *)normals);
    VBO_t tex = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(tex, sizeof(GLfloat) * 5040, (const GLvoid *)texCoords);

    VAO_Attr(sphere, verts, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(sphere, norm, 1, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(sphere, tex, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)(0));

    free(verticies);
    free(normals);
    free(texCoords);
    free(indicies);
    free(tmpVertices);

    return sphere;
}

static VAO_t Sphere_Generate_Method_2(unsigned int *triangle_count)
{
    const double radius = 0.4;
    const int    stacks = 16;
    const int    slices = 32;
    const size_t size   = stacks * (slices + 1) * 2 * 3;

    *triangle_count = (unsigned int)(size / 3);

    GLfloat *sphereVertexArray = malloc(sizeof(GLfloat) * size);
    GLfloat *sphereNormalArray = malloc(sizeof(GLfloat) * size);

    int k = 0;
    for (int j = 0; j < stacks; j++)
    {
        const double latitude1 = (M_PI / stacks) * j - M_PI / 2.0;
        const double latitude2 = (M_PI / stacks) * (j + 1) - M_PI / 2;
        const double sinLat1   = sin(latitude1);
        const double cosLat1   = cos(latitude1);
        const double sinLat2   = sin(latitude2);
        const double cosLat2   = cos(latitude2);
        for (int i = 0; i <= slices; i++)
        {
            const double longitude   = (2 * M_PI / slices) * i;
            const double sinLong     = sin(longitude);
            const double cosLong     = cos(longitude);
            const double x1          = cosLong * cosLat1;
            const double y1          = sinLong * cosLat1;
            const double z1          = sinLat1;
            const double x2          = cosLong * cosLat2;
            const double y2          = sinLong * cosLat2;
            const double z2          = sinLat2;
            sphereNormalArray[k]     = (GLfloat)x2;
            sphereNormalArray[k + 1] = (GLfloat)y2;
            sphereNormalArray[k + 2] = (GLfloat)z2;
            sphereVertexArray[k]     = (GLfloat)(radius * x2);
            sphereVertexArray[k + 1] = (GLfloat)(radius * y2);
            sphereVertexArray[k + 2] = (GLfloat)(radius * z2);
            k += 3;
            sphereNormalArray[k]     = (GLfloat)x1;
            sphereNormalArray[k + 1] = (GLfloat)y1;
            sphereNormalArray[k + 2] = (GLfloat)z1;
            sphereVertexArray[k]     = (GLfloat)(radius * x1);
            sphereVertexArray[k + 1] = (GLfloat)(radius * y1);
            sphereVertexArray[k + 2] = (GLfloat)(radius * z1);
            k += 3;
        }
    }

    VAO_t sphere = VAO_Create();

    VBO_t verts = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(verts, size * sizeof(float), (const GLvoid *)sphereVertexArray);
    VBO_t norm = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(norm, size * sizeof(float), (const GLvoid *)sphereNormalArray);

    VAO_Attr(sphere, verts, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(sphere, norm, 1, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    return sphere;
}

static VAO_t Sphere_Generate_Method_3(unsigned int *index_count)
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
    *index_count = index;

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

static void SphereDraw(const VAO_t sphere_vao, const unsigned int index_count)
{
    VAO_Bind(sphere_vao);

    // Method 1
    // glDrawArrays(GL_TRIANGLES, 0, 2520);
    // glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);

    // Method 2
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, index_count);

    // Method 3
    glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, index_count);
}

void Sphere_Init()
{
    // glEnable(GL_CULL_FACE);

    // Camera, dont forget this!!
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    // glm_vec3_copy(cam.position, (vec3){0.388311f, 0.006581f, 5.520222f});
    // glm_vec3_copy(cam.orientation, (vec3){-0.522374f, 0.019198f, -0.852500f});
    // cam.pitch = 1.100027f;
    // cam.yaw   = -121.498100f;
    sph.cam = cam;

    Shader_t shader = Shader_Create("../../Examples/_tests/sphere.vs",
                                    "../../Examples/_tests/sphere.fs",
                                    3,
                                    (struct VertexAttribute[]){
                                        {.index = 0, .name = "aPos"},
                                        {.index = 1, .name = "aNormal"},
                                        {.index = 2, .name = "aTex"},
                                    });
    sph.shader      = shader;

    Texture_t grid_texture = Texture_Create("../../Examples/_tests/grid512.bmp", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    sph.tex                = grid_texture;

    // Mesh_t sphere_mesh = Mesh_Load(shader, "../../Examples/_tests/icosphere.obj");
    Mesh_t sphere_mesh = Mesh_Load(shader, "../../Examples/res/shapes/sphere.obj");
    sph.sphere_mesh    = sphere_mesh;

    Shader_Bind(shader);
    Shader_Uniform_Int(shader, "diffuse0", 0);

    unsigned int index_count = 0;
    // VAO_t        sphere      = Sphere_Generate_Method_1(1.0f, 36, 18, &index_count);
    //  VAO_t        sphere      = Sphere_Generate_Method_2(&index_count);
    // VAO_t sphere = Sphere_Generate_Method_3(&index_count);
    // sph.sphere   = sphere;

    sph.index_count = index_count;
}

void Sphere_Update()
{
    Camera_Inputs(&sph.cam);

    Shader_Bind(sph.shader);
    Camera_View_Projection_To_Shader(sph.cam, sph.shader, "camMatrix");

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    Uniform_Mat4("model", model);

    Texture_Bind(sph.tex);
    //  Uniform_Vec3("camPos", sph.cam.position);

    // SphereDraw(sph.sphere, sph.index_count);
    Mesh_Draw(sph.sphere_mesh);
}

void Sphere_OnExit()
{
    Shader_Destroy(&sph.shader);
    VAO_Destroy(sph.sphere);
}
