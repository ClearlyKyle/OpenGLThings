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
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

struct Model
{
    struct VAO vao;
    struct VBO material_vbo;
    struct Shader shader;
    struct Texture tex;

    vec3 model_position;

    GLuint num_indicies;
    GLuint num_of_verticies;

    struct MaterialInfo material_info;
};

struct Mesh
{
    struct Shader shader;
    struct Model *models;

    unsigned int num_models;

    GLuint material_ubo_index;
    GLsizeiptr ubo_size;

    unsigned int tex_count;
    struct Texture *textures;
    struct aiString *texture_names;
    char *tex_names[32]; // array of pointers
};

struct Model Model_Import(const char *file_path, const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
struct Model Model_Import_Shader(const char *file_path, struct Shader ShaderColour);

struct Model Model_ASSIMP();

void Model_Draw(struct Model model, struct Camera cam);
void Model_Free(struct Model model);

struct Mesh Load_Model_Data(const char *file_path);
void Model_Render_Mesh(struct Mesh m, struct Camera cam);
void Mesh_Free(struct Mesh mesh);

#endif // __MODELLOADER_H__