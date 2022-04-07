#ifndef __MODELLOADER_H__
#define __MODELLOADER_H__

#include "util.h"

#include "VAO.h"
#include "EBO.h"
#include "Shaders.h"
#include "Camera.h"

struct Model
{
    struct VAO vao;
    struct Shader shader;
    struct Texture tex;

    vec3 model_position;

    GLuint num_indicies;
    GLuint num_of_verticies;
};

struct MaterialInfo
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

struct Mesh
{
    struct Shader shader;
    struct Model *models;
    unsigned int num_models;
};

struct Model Model_Import(const char *file_path, const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
struct Model Model_Import_Shader(const char *file_path, struct Shader ShaderColour);

struct Model Model_ASSIMP();

void Model_Draw(struct Model model, struct Camera cam);
void Model_Free(struct Model model);

struct Mesh Load_Model_Data(const char *file_path);
void Mesh_Free(struct Mesh mesh);

#endif // __MODELLOADER_H__