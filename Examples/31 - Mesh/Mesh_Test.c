#include "Mesh_Test.h"

static struct MeshTest
{
    Camera_t cam;

    Shader_t shader;
    MMesh_t  mesh;
} mt;

#define NUM_OF_ELEMETS(A) (sizeof((A)) / sizeof((A))[0])

void MeshTest_Init()
{
    const GLfloat plane_vertices[] =
        {
            //     COORDINATES  / NORMALS /     COLORS      / TexCoord  //
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    const GLuint plane_indicies[] =
        {
            0, 1, 2, // Upper triangle
            0, 2, 3  // Lower triangle
        };

    const char *diffuse_file_path      = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_DIFFUSE.png";
    const char *specular_file_path     = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_SPECULAR.png";
    const char *normal_file_path       = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_NORMAL.png";
    const char *displacement_file_path = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_DISP.png";

    struct Texture diff_tex = Texture_Create(diffuse_file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    struct Texture spec_tex = Texture_Create(specular_file_path, GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
    struct Texture nrm_tex  = Texture_Create(normal_file_path, GL_TEXTURE_2D, 2, GL_RGB, GL_UNSIGNED_BYTE);
    struct Texture disp_tex = Texture_Create(displacement_file_path, GL_TEXTURE_2D, 3, GL_RED, GL_UNSIGNED_BYTE);

    const struct Shader plane_shader = Shader_Create(
        "../../Examples/31 - Mesh/plane_shader.vs",
        "../../Examples/31 - Mesh/plane_shader.fs",
        4, (struct VertexAttribute[]){
               {.index = 0, .name = "aPos"},
               {.index = 1, .name = "aNormal"},
               {.index = 2, .name = "aColor"},
               {.index = 3, .name = "aTex"},
           });

    Shader_Bind(plane_shader);

    Texture_t textures[] = {
        diff_tex};

    MMesh_t mesh = Mesh_Construct(plane_vertices,
                                  plane_indicies,
                                  NUM_OF_ELEMETS(plane_indicies),
                                  NUM_OF_ELEMETS(textures),
                                  textures);
    mt.mesh      = mesh;
    mt.shader    = plane_shader;
}

void MeshTest_Update()
{
    Camera_Inputs(&mt.cam);

    MMesh_Draw(mt.mesh, mt.shader, mt.cam);
}

void MeshTest_OnExit()
{
    Camera_Print_Values(mt.cam);
}
