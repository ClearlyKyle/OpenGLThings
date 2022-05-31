#include "Mesh.h"

enum vertex_attributes
{
    MESH_POSTION,
    MESH_NORMALS,
    MESH_COLOURS,
    MESH_TEX_COORDS,
    MESH_ATTRIBUTE_COUNT
};

MMesh_t Mesh_Construct(const GLfloat *verticies,
                       const GLuint  *indicies,
                       const GLsizei  num_indicies,
                       const GLsizei  num_textures,
                       Texture_t     *textures)
{
    VAO_t VAO = VAO_Create();
    VBO_t VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(VBO, sizeof(verticies), (const GLvoid *)(verticies));

    EBO_t EBO = EBO_Create();
    EBO_Buffer(EBO, sizeof(indicies), (const GLvoid *)(indicies));

    VAO_Attr(VAO, VBO, 0, sizeof(verticies), GL_FLOAT, 3, (const GLvoid *)(0));
    VAO_Attr(VAO, VBO, 1, sizeof(verticies), GL_FLOAT, 3, (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(VAO, VBO, 2, sizeof(verticies), GL_FLOAT, 3, (const GLvoid *)(6 * sizeof(GLfloat)));
    VAO_Attr(VAO, VBO, 3, sizeof(verticies), GL_FLOAT, 2, (const GLvoid *)(9 * sizeof(GLfloat)));

    vec3   translation = {0.0f, 0.0f, 0.0f};
    versor rotation    = {0.0f, 0.0f, 0.0f, 0.0f};
    vec3   scale       = {1.0f, 1.0f, 1.0f};

    mat4 object_translation = {0};
    mat4 object_rotaion     = {0};
    mat4 object_scale       = {0};
    mat4 object_matrix      = GLM_MAT4_IDENTITY_INIT;

    glm_translate_make(object_translation, translation);
    glm_quat_mat4(rotation, object_rotaion);
    glm_scale_make(object_scale, scale);

    MMesh_t return_mesh = {
        .vao          = VAO,
        .num_indicies = num_indicies,
        .textures     = textures,
        .num_textures = num_textures};

    glm_mat4_copy(object_translation, return_mesh.translation);
    glm_mat4_copy(object_rotaion, return_mesh.rotation);
    glm_mat4_copy(object_scale, return_mesh.scale);
    glm_mat4_copy(object_matrix, return_mesh.matrix);

    return return_mesh;
}

void MMesh_Draw(const MMesh_t mesh, const Shader_t shader, const Camera_t camera)
{
    Shader_Bind(shader);
    VAO_Bind(mesh.vao);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "translation", mesh.translation);
    Shader_Uniform_Mat4(shader, "rotation", mesh.rotation);
    Shader_Uniform_Mat4(shader, "scale", mesh.scale);
    Shader_Uniform_Mat4(shader, "model", mesh.matrix);
    Shader_Uniform_Int(shader, "tex0", 0);

    for (Texture_t *p = mesh.textures, *end = &mesh.textures[mesh.num_textures]; p != end; p++)
    {
        Texture_Bind(*p);
    }

    glDrawElements(GL_TRIANGLES, mesh.num_indicies, GL_UNSIGNED_INT, 0);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
