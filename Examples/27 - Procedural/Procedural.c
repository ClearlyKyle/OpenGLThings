#include "Procedural.h"
#include <time.h>

static struct Procedural
{
    Camera_t cam;

    Shader_t circle_Shader;
    VAO_t    circle_VAO;
    GLuint   circle_Count;

    Shader_t line_Shader;
    VAO_t    line_VAO;
    GLuint   line_Indicies;

    Shader_t plane_Shader;
    VAO_t    plane_VAO;
    GLuint   plane_Indicies;

} pc;

static float FLOAT_RAND2(const float min, const float max)
{
    const float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);             /* [min, max] */
}

#define PI 3.141592653589793238462643383279502884197 // 39dp
#define NUM_OF_CIRCLE_TRIANGLES 100

GLfloat *Circle_Verticies(const GLuint num_of_triangles, const float radius)
{
    // const GLfloat random_center_x = FLOAT_RAND2(-0.5f, 0.5f);
    // const GLfloat random_center_y = FLOAT_RAND2(-0.5f, 0.5f);
    const GLfloat random_center_x = 0.0f;
    const GLfloat random_center_y = 0.0f;
    // const GLfloat random_center_z = FLOAT_RAND(-0.5f, 0.5f);

    GLfloat *verticies = (GLfloat *)malloc(sizeof(*verticies) * num_of_triangles * 3); // 3 because X Y Z position

    float angle = 0.0f;
    for (GLuint i = 0; i < num_of_triangles; i++)
    {
        angle += (2.0f * ((float)PI / 100.0f));

        verticies[i * 3 + 0] = (radius * cosf(angle)) + random_center_x;
        verticies[i * 3 + 1] = (radius * sinf(angle)) + random_center_y;
        verticies[i * 3 + 2] = 0.0f;
    }

    return verticies;
}

GLuint *Circle_Indicies(const GLuint num_of_triangles)
{
    GLuint *indicies = (GLuint *)malloc(sizeof(GLuint) * num_of_triangles);

    for (GLuint i = 0; i < num_of_triangles; i++)
    {
        indicies[i] = i;
    }

    return indicies;
}

int getVerticesCount(int width, int height)
{
    return width * height * 3;
}

int getIndicesCount(int width, int height)
{
    return (width * height) + (width - 1) * (height - 2);
}

GLfloat *get_plane_vertices(int width, int height)
{
    GLfloat *vertices = (GLfloat *)malloc(sizeof(GLfloat) * (width * height * 3));

    int i = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            vertices[i++] = (GLfloat)col;
            vertices[i++] = (GLfloat)row;
            vertices[i++] = 0.0f;
        }
    }

    return vertices;
}

GLuint *get_plane_Indices(GLuint width, GLuint height, GLuint *num_of_indicies_return)
{
    //*num_of_indicies_return = (width * height) + (width - 1) * (height - 2);
    //*num_of_indicies_return = (width + 1) * (height + 1);

    const int numIndPerRow    = width - 1;
    const int numIndDegensReq = (height - 1) * 2;
    const int total_indices   = numIndPerRow * height + numIndDegensReq;

    // Now build the index data
    const int numStripsRequired = width - 1;
    const int numDegensRequired = (2 * (numStripsRequired + 2));
    const int verticesPerStrip  = 2 * height;

    *num_of_indicies_return = (verticesPerStrip * numStripsRequired) + numDegensRequired;
    //*num_of_indicies_return = 2 * (width - 1) * (height - 1);
    //*num_of_indicies_return = (width * height) + (width - 1) * (height - 2);

    GLuint *indices = (GLuint *)malloc(sizeof(GLuint) * (*num_of_indicies_return));

    unsigned int offset = 0;

    // indices[offset++] = width - 1;
    // indices[offset++] = width - 1;
    // indices[offset++] = 0;
    indices[offset++] = width - 1;
    indices[offset++] = 0;
    indices[offset++] = 0;

    for (unsigned int y = 0; y < width - 1; y++)
    {
        if (y > 0)
        {
            // Degenerate begin: repeat first vertex
            indices[offset++] = (y * width);
        }

        for (unsigned int x = 0; x < height; x++)
        {
            // One part of the strip
            indices[offset++] = ((y * width) + x);
            indices[offset++] = (((y + 1) * width) + x);
        }

        if (y < width - 2)
        {
            // Degenerate end: repeat last vertex
            indices[offset++] = (((y + 1) * width) + (height - 1));
        }
    }

    indices[offset++] = (height * width) - 1;
    indices[offset++] = (height * width) - 1;
    indices[offset++] = (height * width) - width;
    // indices[offset++] = (height * width) - 1;
    // indices[offset++] = (height * width) - width;
    // indices[offset++] = (height * width) - width;

    for (size_t i = 0; i < *num_of_indicies_return; i++)
    {
        printf("%d\n", indices[i]);
    }

    return indices;
}

GLfloat *Plane_Verticies(vec3 v0, vec3 v1, vec3 v2, vec3 v3, const GLuint divisions)
{
    // v3 --- v2
    // |      |
    // |      |
    // v0 --- v1

    vec3 vec03;
    vec3 vec12;
    glm_vec3_sub(v3, v0, vec03);
    glm_vec3_sub(v2, v1, vec12);

    glm_vec3_divs(vec03, (float)divisions, vec03);
    glm_vec3_divs(vec12, (float)divisions, vec12);

    //                                            POSITIONS(3) TEX(2)
    GLfloat *plane = (GLfloat *)malloc((sizeof(GLfloat) * 3) * (divisions + 1) * (divisions + 1));
    // GLfloat *plane = (GLfloat *)malloc(sizeof(GLfloat) * 3 + sizeof(GLfloat) * 2);

    int i = 0;
    for (unsigned int row = 0; row < divisions + 1; row++)
    {
        vec3 start = {
            v0[0] + (vec03[0] * row),
            v0[1] + (vec03[1] * row),
            v0[2] + (vec03[2] * row),
        };

        vec3 end = {
            v1[0] + (vec12[0] * row),
            v1[1] + (vec12[1] * row),
            v1[2] + (vec12[2] * row),
        };

        vec3 vector_div = {
            (end[0] - start[0]) / (float)divisions,
            (end[1] - start[1]) / (float)divisions,
            (end[2] - start[2]) / (float)divisions,
        };

        for (unsigned int col = 0; col < divisions + 1; col++)
        {
            vec3 current_vec = {
                start[0] + (vector_div[0] * col),
                start[1] + (vector_div[1] * col),
                start[2] + (vector_div[2] * col),
            };

            const int index  = 3 * (row * (divisions + 1) + col);
            plane[index + 0] = current_vec[0];
            plane[index + 1] = current_vec[1];
            plane[index + 2] = current_vec[2];
        }
    }

    return plane;
}

GLuint *Plane_Indicies(const GLuint divisions)
{
    return NULL;
}

GLfloat *Line_Vertices(vec3 start, vec3 end, const GLuint divison)
{
    GLfloat *verts_output = (GLfloat *)malloc((sizeof(GLfloat) * 3) * (divison + 1));

    vec3 vec_divison = {
        (end[0] - start[0]) / (float)divison,
        (end[1] - start[1]) / (float)divison,
        (end[2] - start[2]) / (float)divison,
    };

    for (unsigned int i = 0; i < divison + 1; i++)
    {
        vec3 current_vec = {
            start[0] + (vec_divison[0] * i),
            start[1] + (vec_divison[1] * i),
            start[2] + (vec_divison[2] * i),
        };

        verts_output[3 * i + 0] = current_vec[0];
        verts_output[3 * i + 1] = current_vec[1];
        verts_output[3 * i + 2] = current_vec[2];
    }

    return verts_output;
}

GLuint *Line_Indices(const GLuint divison)
{
    GLuint *indice_output = (GLuint *)malloc(sizeof(GLuint) * (divison + 1));

    for (GLuint i = 0; i < divison + 1; i++)
    {
        indice_output[i] = i;
    }

    return indice_output;
}

void Procedural_Init()
{
    srand((unsigned int)time(NULL));

    { // CIRCLE
        const Shader_t circle_Shader = Shader_Create("../../Examples/27 - Procedural/circle.vs",
                                                     "../../Examples/27 - Procedural/circle.fs",
                                                     1,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"},
                                                     });

        const GLuint num_of_triangles = 100;
        const float  circle_radius    = 0.5f;

        GLfloat *verticies = Circle_Verticies(num_of_triangles, circle_radius);
        GLuint  *indicies  = Circle_Indicies(num_of_triangles);

        VAO_t circle_VAO = VAO_Create();
        VBO_t circle_VBO = VBO_Create(GL_ARRAY_BUFFER);
        EBO_t cricle_EBO = EBO_Create();

        VBO_Buffer(circle_VBO, sizeof(verticies) * num_of_triangles * 3, (const GLvoid *)verticies);
        EBO_Buffer(cricle_EBO, sizeof(indicies) * num_of_triangles, (const GLvoid *)indicies);

        VAO_Attr(circle_VAO, circle_VBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)0);

        free(verticies);
        free(indicies);

        pc.circle_Shader = circle_Shader;
        pc.circle_VAO    = circle_VAO;
        pc.circle_Count  = num_of_triangles;
    } // CIRCLE

    { // LINE
        const Shader_t line_shader = Shader_Create("../../Examples/27 - Procedural/line.vs",
                                                   "../../Examples/27 - Procedural/line.fs",
                                                   1,
                                                   (struct VertexAttribute[]){
                                                       {.index = 0, .name = "aPos"},
                                                   });

        const GLuint divisons = 10;
        vec3         start    = {-0.5f, 0.5f, 0.0f};
        vec3         end      = {0.5f, 0.5f, 0.0f};

        GLfloat *vertices = Line_Vertices(start, end, divisons);
        GLuint  *indices  = Line_Indices(divisons);

        const GLuint num_of_verts = divisons + 1;

        VAO_t line_VAO = VAO_Create();

        VBO_t line_VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(line_VBO, sizeof(GLfloat) * 3 * num_of_verts, (const GLvoid *)vertices);

        EBO_t line_EBO = EBO_Create();
        EBO_Buffer(line_EBO, sizeof(GLuint) * num_of_verts, (const GLvoid *)indices);

        VAO_Attr(line_VAO, line_VBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)0);

        free(vertices);
        free(indices);

        pc.line_Shader   = line_shader;
        pc.line_VAO      = line_VAO;
        pc.line_Indicies = num_of_verts;

    } // LINE

    { // PLANE
        const Shader_t plane_Shader = Shader_Create("../../Examples/27 - Procedural/plane.vs",
                                                    "../../Examples/27 - Procedural/plane.fs",
                                                    1,
                                                    (struct VertexAttribute[]){
                                                        {.index = 0, .name = "aPos"},
                                                    });

        const GLuint num_of_divisons = 16;
        const float  circle_radius   = 0.5f;

        // v3 --- v2
        // |      |
        // |      |
        // v0 --- v1
        vec3 v0 = {-1.0f, 2.0f, 1.0f};
        vec3 v1 = {1.0f, 0.0f, 1.0f};
        vec3 v2 = {1.0f, 0.0f, -1.0f};
        vec3 v3 = {-1.0f, 0.0f, -1.0f};

        GLuint num_of_verts = (num_of_divisons + 1) * (num_of_divisons + 1);
        GLuint num_of_indi;

        GLfloat *verticies = Plane_Verticies(v3, v2, v1, v0, num_of_divisons);
        GLuint  *indicies  = get_plane_Indices(num_of_divisons + 1, num_of_divisons + 1, &num_of_indi);

        VAO_t plane_VAO = VAO_Create();

        VBO_t plane_VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(plane_VBO, sizeof(GLfloat) * num_of_verts * 3, (const GLvoid *)verticies);

        EBO_t plane_EBO = EBO_Create();
        EBO_Buffer(plane_EBO, sizeof(GLuint) * num_of_indi, (const GLvoid *)indicies);

        VAO_Attr(plane_VAO, plane_VBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)0);

        free(verticies);
        free(indicies);

        const Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
        pc.cam             = cam;

        pc.plane_Shader   = plane_Shader;
        pc.plane_VAO      = plane_VAO;
        pc.plane_Indicies = num_of_indi;
    } // PLANE
}

void Procedural_Update()
{
    //{ // CIRCLE
    //    Shader_Bind(pc.circle_Shader);
    //    VAO_Bind(pc.circle_VAO);
    //    glDrawArrays(GL_TRIANGLE_FAN, 0, pc.circle_Count);
    //} // CIRCLE

    //{ // LINE
    //    Shader_Bind(pc.line_Shader);
    //    VAO_Bind(pc.line_VAO);
    //    // glDrawArrays(GL_POINTS, 0, pc.line_Indicies);
    //    glDrawElements(GL_POINTS, pc.line_Indicies, GL_UNSIGNED_INT, 0);
    //} // LINE

    { // PLANE
        Camera_Inputs(&pc.cam);

        Shader_Bind(pc.plane_Shader);

        Camera_View_Projection_To_Shader(pc.cam, pc.plane_Shader, "camMatrix");

        VAO_Bind(pc.plane_VAO);
        // glDrawArrays(GL_LINE_STRIP, 0, pc.plane_Indicies);
        glDrawElements(GL_TRIANGLE_STRIP, pc.plane_Indicies, GL_UNSIGNED_INT, 0);

    } // PLANE
}

void Procedural_OnExit()
{
    Camera_Print_Values(pc.cam);

    Shader_Destroy(&pc.circle_Shader);
    VAO_Destroy(pc.circle_VAO);
    Shader_Destroy(&pc.line_Shader);
    VAO_Destroy(pc.line_VAO);
    Shader_Destroy(&pc.plane_Shader);
    VAO_Destroy(pc.plane_VAO);
}
