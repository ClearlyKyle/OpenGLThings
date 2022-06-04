#include "Procedural.h"
#include <time.h>

static struct Procedural
{
    Camera_t cam;

    Shader_t circle_Shader;
    VAO_t    circle_VAO;
    GLuint   circle_Count;
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

    glm_vec3_divs(vec03, divisions, vec03);
    glm_vec3_divs(vec12, divisions, vec12);

    //                                 POSITIONS             TEX
    GLfloat *plane = (GLfloat *)malloc(sizeof(GLfloat) * 3);
    // GLfloat *plane = (GLfloat *)malloc(sizeof(GLfloat) * 3 + sizeof(GLfloat) * 2);

    for (unsigned int row = 0; row < divisions + 1; row++)
    {
        vec3 start = {
            v0[0] + vec03[0] * row,
            v0[1] + vec03[1] * row,
            v0[2] + vec03[2] * row,
        };

        vec3 end = {
            v1[0] + vec12[0] * row,
            v1[1] + vec12[1] * row,
            v1[2] + vec12[2] * row,
        };

        vec3 vector_div = {
            (end[0] + start[0]) / divisions,
            (end[1] + start[1]) / divisions,
            (end[2] + start[2]) / divisions,
        };

        for (unsigned int col = 0; col < divisions + 1; col++)
        {
            vec3 current_vec = {
                start[0] + vector_div[0] * col,
                start[1] + vector_div[1] * col,
                start[2] + vector_div[2] * col,
            };

            plane[(divisions * col + row) + 0] = current_vec[0];
            plane[(divisions * col + row) + 1] = current_vec[1];
            plane[(divisions * col + row) + 2] = current_vec[2];
        }
    }
}

GLuint *Plane_Indicies(const GLuint divisions)
{
    
}

void Procedural_Init()
{
    srand((unsigned int)time(NULL));

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

    pc.circle_Shader = circle_Shader;
    pc.circle_VAO    = circle_VAO;
    pc.circle_Count  = num_of_triangles;
}

void Procedural_Update()
{
    Shader_Bind(pc.circle_Shader);

    VAO_Bind(pc.circle_VAO);

    glDrawArrays(GL_TRIANGLE_FAN, 0, pc.circle_Count);
}

void Procedural_OnExit()
{
    Camera_Print_Values(pc.cam);

    Shader_Destroy(&pc.circle_Shader);
    VAO_Destroy(pc.circle_VAO);
}
