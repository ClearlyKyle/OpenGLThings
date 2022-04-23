#ifndef __MODELLOADER_H__
#define __MODELLOADER_H__

#include "util.h"

#include "VAO.h"
#include "UBO.h"
#include "EBO.h"
#include "Shaders.h"
#include "Camera.h"

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct MaterialInfo
{
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emission;
    float shininess;
};

struct Model
{
    struct VAO     vao;
    struct VBO     material_vbo;
    struct Shader  shader;
    struct Texture tex;

    vec3 model_position;

    GLuint num_indicies;
    GLuint num_of_verticies;

    struct MaterialInfo material_info;
};

struct TextureInfo
{
    struct Texture *textures;

    // How many textures currently loaded into texture_file_paths
    unsigned int tex_count;

    // All unique texture file paths will be added here
    char *texture_file_paths[32]; // array of pointers

    enum aiTextureType texture_type[32];
};

struct InstancingInfo
{
    GLuint count;

    // vec4 *translation;
    // vec4 *rotation;
    // vec4 *scale;

    mat4 *matrix;
};

struct Mesh
{
    struct aiScene *scene;

    struct Shader shader;
    struct Model *models;

    unsigned int num_models;

    struct InstancingInfo instancing;

    GLuint     material_ubo_index;
    GLsizeiptr ubo_size;

    struct TextureInfo texInfo;

    // Default values
    mat4 matrix;
    vec3 translation;
    vec4 rotation;
    vec3 scale;
};

typedef struct Mesh Mesh_t;

// "tinyobj_parse_obj" methods
struct Model Model_Import(const char *file_path, const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
struct Model Model_Import_Shader(const char *file_path, struct Shader ShaderColour);

void Model_Draw(struct Model model, struct Camera cam);
void Model_Free(struct Model model);

void Mesh_Set_Translation(struct Mesh *mesh, vec3 new_translation);
void Mesh_Set_Rotation(struct Mesh *mesh, vec4 new_rotation);
void Mesh_Set_Scale(struct Mesh *mesh, vec3 new_scale);

// "assimp" methods
Mesh_t Mesh_Load(const struct Shader shader, const char *file_path);

Mesh_t Mesh_Load_Instancing(const struct Shader shader, GLuint count, mat4 *matrix, const char *file_path);
void   Mesh_Instance_Load_Vectors(Mesh_t *mesh, GLuint index, vec3 trans, versor rot, vec3 scale);

void Mesh_Draw(struct Mesh m);
void Mesh_Free(struct Mesh mesh);

#endif // __MODELLOADER_H__