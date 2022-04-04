#include "ModelLoader.h"

// TINY OBJ
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj/tinyobj_loader_c.h"

static void _Load_File(void *ctx, const char *filename, const int is_mtl, const char *obj_filename, char **buffer, size_t *len)
{
    size_t string_size = 0, read_size = 0;
    FILE *handler;
    errno_t err;

    // Open for read (will fail if file "crt_fopen_s.c" doesn't exist)
    err = fopen_s(&handler, filename, "r");
    if (err != 0)
    {
        fprintf(stderr, "[loadFile] File : %s was NOT opened!\n", filename);
        exit(1);
    }

    if (handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        *buffer = (char *)malloc(sizeof(char) * (string_size + 1));
        read_size = fread(*buffer, sizeof(char), (size_t)string_size, handler);
        (*buffer)[string_size] = '\0';
        if (string_size != read_size)
        {
            free(buffer);
            *buffer = NULL;
        }
        fclose(handler);
    }

    *len = read_size;
}

static void _Print_TinyObj_Info(tinyobj_material_t *material, tinyobj_attrib_t *attrib)
{
    fprintf(stderr, "\ntinyobj_material_t ____________\n");
    fprintf(stderr, "name : %s\n", material->name);

    fprintf(stderr, "ambient  \t\t: {%f, %f, %f}\n", material->ambient[0], material->ambient[1], material->ambient[2]);
    fprintf(stderr, "diffuse  \t\t: {%f, %f, %f}\n", material->diffuse[0], material->diffuse[1], material->diffuse[2]);
    fprintf(stderr, "specular \t\t: {%f, %f, %f}\n", material->specular[0], material->specular[1], material->specular[2]);
    fprintf(stderr, "transmittance \t\t: {%f, %f, %f}\n", material->transmittance[0], material->transmittance[1], material->transmittance[2]);
    fprintf(stderr, "emission \t\t: {%f, %f, %f}\n", material->emission[0], material->emission[1], material->emission[2]);

    fprintf(stderr, "shininess   \t\t: %f\n", material->shininess);
    fprintf(stderr, "ior   \t\t\t: %f\n", material->ior);
    fprintf(stderr, "dissolve   \t\t: %f\n", material->dissolve);

    fprintf(stderr, "map_Ka   \t\t: %s\n", material->ambient_texname);
    fprintf(stderr, "map_Kd   \t\t: %s\n", material->diffuse_texname);
    fprintf(stderr, "map_Ks   \t\t: %s\n", material->specular_texname);
    fprintf(stderr, "map_Ns   \t\t: %s\n", material->specular_highlight_texname);

    fprintf(stderr, "map_bump \t\t: %s\n", material->bump_texname);
    fprintf(stderr, "disp     \t\t: %s\n", material->displacement_texname);
    fprintf(stderr, "map_d    \t\t: %s\n", material->alpha_texname);

    fprintf(stderr, "\ntinyobj_attrib_t ____________\n");
    fprintf(stderr, "num_vertices   \t\t: %d\n", attrib->num_vertices);
    fprintf(stderr, "num_normals    \t\t: %d\n", attrib->num_normals);
    fprintf(stderr, "num_texcoords  \t\t: %d\n", attrib->num_texcoords);
    fprintf(stderr, "num_faces      \t\t: %d\n", attrib->num_faces);
    fprintf(stderr, "num_face_num_verts\t: %d (number of 'f' rows)\n", attrib->num_face_num_verts);
}

struct Model Model_Import(const char *file_path, const char *vertex_shader_path, const char *fragment_shader_path, size_t number_of_attributes, struct VertexAttribute attributes[])
{
    struct Model m;

    tinyobj_shape_t *shape = NULL;
    tinyobj_material_t *material = NULL;
    tinyobj_attrib_t attrib;

    size_t num_shapes;
    size_t num_materials;

    tinyobj_attrib_init(&attrib);

    const int ret = tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, file_path, _Load_File, NULL, TINYOBJ_FLAG_TRIANGULATE);
    if (ret != TINYOBJ_SUCCESS)
    {
        fprintf(stderr, "ERROR!\n");
        exit(1);
    }

    _Print_TinyObj_Info(material, &attrib);

    m.num_indicies = attrib.num_face_num_verts * 3;
    m.num_of_verticies = m.num_indicies;

    // VERTICIES
    const GLsizeiptr vertex_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat *vertex_data = (GLfloat *)malloc(vertex_data_size);

    // NORMALS
    const GLsizeiptr normal_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat *normal_data = (GLfloat *)malloc(normal_data_size);

    // TEXTURES
    const GLsizeiptr texture_data_size = sizeof(GLfloat) * ((m.num_indicies) * 2);
    GLfloat *texture_data = (GLfloat *)malloc(texture_data_size);

    const GLsizeiptr index_data_size = sizeof(unsigned int) * (m.num_indicies);
    unsigned int *index_data = (unsigned int *)malloc(index_data_size);

    for (unsigned int i = 0; i < attrib.num_face_num_verts; i++)
    {
        // f v1/vt1/vn1

        // VERTICIES
        const unsigned int vert_index1 = attrib.faces[3 * i + 0].v_idx;
        const unsigned int vert_index2 = attrib.faces[3 * i + 1].v_idx;
        const unsigned int vert_index3 = attrib.faces[3 * i + 2].v_idx;

        const unsigned int normal_index1 = attrib.faces[3 * i + 0].vn_idx;
        const unsigned int normal_index2 = attrib.faces[3 * i + 1].vn_idx;
        const unsigned int normal_index3 = attrib.faces[3 * i + 2].vn_idx;

        const unsigned int texture_index1 = attrib.faces[3 * i + 0].vt_idx;
        const unsigned int texture_index2 = attrib.faces[3 * i + 1].vt_idx;
        const unsigned int texture_index3 = attrib.faces[3 * i + 2].vt_idx;

        index_data[3 * i + 0] = vert_index1;
        index_data[3 * i + 1] = vert_index2;
        index_data[3 * i + 2] = vert_index3;

        // VERTICIES
        vertex_data[(i * 9) + 0] = attrib.vertices[vert_index1 * 3 + 0]; // V1
        vertex_data[(i * 9) + 1] = attrib.vertices[vert_index1 * 3 + 1];
        vertex_data[(i * 9) + 2] = attrib.vertices[vert_index1 * 3 + 2];
        vertex_data[(i * 9) + 3] = attrib.vertices[vert_index2 * 3 + 0]; // V2
        vertex_data[(i * 9) + 4] = attrib.vertices[vert_index2 * 3 + 1];
        vertex_data[(i * 9) + 5] = attrib.vertices[vert_index2 * 3 + 2];
        vertex_data[(i * 9) + 6] = attrib.vertices[vert_index3 * 3 + 0]; // V3
        vertex_data[(i * 9) + 7] = attrib.vertices[vert_index3 * 3 + 1];
        vertex_data[(i * 9) + 8] = attrib.vertices[vert_index3 * 3 + 2];

        // NORMALS
        normal_data[(i * 9) + 0] = attrib.normals[normal_index1 * 3 + 0];
        normal_data[(i * 9) + 1] = attrib.normals[normal_index1 * 3 + 1];
        normal_data[(i * 9) + 2] = attrib.normals[normal_index1 * 3 + 2];
        normal_data[(i * 9) + 3] = attrib.normals[normal_index2 * 3 + 0];
        normal_data[(i * 9) + 4] = attrib.normals[normal_index2 * 3 + 1];
        normal_data[(i * 9) + 5] = attrib.normals[normal_index2 * 3 + 2];
        normal_data[(i * 9) + 6] = attrib.normals[normal_index3 * 3 + 0];
        normal_data[(i * 9) + 7] = attrib.normals[normal_index3 * 3 + 1];
        normal_data[(i * 9) + 8] = attrib.normals[normal_index3 * 3 + 2];

        // TEXTURES
        texture_data[(i * 6) + 0] = attrib.texcoords[texture_index1 * 2 + 0];
        texture_data[(i * 6) + 1] = attrib.texcoords[texture_index1 * 2 + 1];
        texture_data[(i * 6) + 2] = attrib.texcoords[texture_index2 * 2 + 0];
        texture_data[(i * 6) + 3] = attrib.texcoords[texture_index2 * 2 + 1];
        texture_data[(i * 6) + 4] = attrib.texcoords[texture_index3 * 2 + 0];
        texture_data[(i * 6) + 5] = attrib.texcoords[texture_index3 * 2 + 1];
    }

    printf("Loaded coordinates!\n");
    // for (size_t i = 0; i < attrib.num_vertices; i++)
    //{
    //     printf("vert  [%lld] {%f, %f, %f} {%f, %f, %f}\n", i, vertex_data[i * 6 + 0], vertex_data[i * 6 + 1], vertex_data[i * 6 + 2],
    //            vertex_data[i * 6 + 3], vertex_data[i * 6 + 4], vertex_data[i * 6 + 5]);
    // }

    // for (size_t i = 0; i < m.num_indicies; i += 1)
    //{
    //     printf("index [%lld] {%d}\n", i, index_data[i]);
    // }

    // Shader
    struct Shader shader = Shader_Create(
        vertex_shader_path,
        fragment_shader_path,
        number_of_attributes,
        attributes);

    // Load the data to the GPU
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    m.vao = vao;

    struct VBO vbo_verticies = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo_verticies, vertex_data_size, (const GLvoid *)vertex_data);

    struct VBO vbo_normals = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo_normals, normal_data_size, (const GLvoid *)normal_data);

    struct VBO vbo_textures = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo_textures, texture_data_size, (const GLvoid *)texture_data);

    // struct EBO ebo = EBO_Create();
    // EBO_Buffer(ebo, index_data_size, (const GLvoid *)index_data);

    VAO_Attr(vao, vbo_verticies, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(vao, vbo_normals, 1, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(vao, vbo_textures, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (const GLvoid *)(0));

    vec3 model_position = {0.0f, 0.0f, -2.0f};
    mat4 model_transform;
    glm_translate_make(model_transform, model_position);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "model", model_transform);

    // TEXTURES
    const char *texture_file_path = "../../res/models/Dog House/Doghouse_PBR_BaseColor.png";
    struct Texture tex = Texture_Create(texture_file_path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    m.tex = tex;

    Texture_Uniform(shader, tex, "tex0", 0);

    m.shader = shader;
    VBO_Unbind();
    VAO_Unbind();

    fprintf(stderr, "Loading model complete : %s\n", file_path);

    return m;
}

void Model_Free(struct Model model)
{
    Shader_Destroy(model.shader);
    VAO_Destroy(model.vao);
}
