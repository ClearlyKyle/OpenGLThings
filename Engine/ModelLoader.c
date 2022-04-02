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

static void _Get_Vertex_Indicies(const tinyobj_attrib_t *attrib)
{
    // Check if mesh is triangulated
    const unsigned int triangulated = (unsigned int)ceil((float)attrib->num_faces / (float)attrib->num_face_num_verts);
    fprintf(stderr, "triangulated = %d\n", triangulated);

    float *vert_indicies = NULL;
    float *uv_coords = NULL;
    float *normal_coords = NULL;

    if (triangulated == 3)
    {
        // 3 face values
        fprintf(stderr, "Using 3 face values setup...\n");
        const unsigned int number_of_f_values = attrib->num_faces / 3;
        const unsigned int number_of_triangles = number_of_f_values;

        fprintf(stderr, "number_of_f_values  : %d\n", number_of_f_values);
        fprintf(stderr, "number_of_triangles : %d\n", number_of_triangles);

        vert_indicies = (float *)malloc(sizeof(float) * (number_of_triangles * 3) * 4); // 3 verts per triangle, each with 3 coordinates
        if (!vert_indicies)
        {
            fprintf(stderr, "Error alocating memeory for 'vert_coords'\n");
            exit(1);
        }
        // uv_coords = (float *)malloc(sizeof(float) * (number_of_triangles * 3) * 2);
        // if (!uv_coords)
        //{
        //     fprintf(stderr, "Error alocating memeory for 'vert_coords'\n");
        //     exit(1);
        // }
        // normal_coords = (float *)malloc(sizeof(float) * (number_of_triangles * 3) * 4);
        // if (!normal_coords)
        //{
        //     fprintf(stderr, "Error alocating memeory for 'normal_coords'\n");
        //     exit(1);
        // }

        for (size_t i = 0; i < number_of_f_values; i++)
        {
            const unsigned int f_vert_index1 = attrib->faces[3 * i + 0].v_idx;
            const unsigned int f_vert_index2 = attrib->faces[3 * i + 1].v_idx;
            const unsigned int f_vert_index3 = attrib->faces[3 * i + 2].v_idx;

            // verts
            vert_indicies[(i * 12) + 0] = attrib->vertices[f_vert_index1 * 3 + 0]; // X
            vert_indicies[(i * 12) + 1] = attrib->vertices[f_vert_index1 * 3 + 1]; // Y
            vert_indicies[(i * 12) + 2] = attrib->vertices[f_vert_index1 * 3 + 2]; // Z
        }
    }
    else if (triangulated == 4)
    {
    }
    else
    {
        fprintf(stderr, "[_Get_Vertex_Indicies] Some kind of triangulation error!\n");
    }
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

    const GLsizeiptr vertex_data_size = sizeof(GLfloat) * ((attrib.num_vertices + attrib.num_normals) * 3);
    GLfloat *vertex_data = (GLfloat *)malloc(vertex_data_size);

    const GLsizeiptr index_data_size = sizeof(unsigned int) * (m.num_indicies);
    unsigned int *index_data = (unsigned int *)malloc(index_data_size);

    for (unsigned int i = 0; i < attrib.num_face_num_verts; i++)
    {
        // f v1/vt1/vn1
        const unsigned int vert_index1 = attrib.faces[3 * i + 0].v_idx;
        const unsigned int vert_index2 = attrib.faces[3 * i + 1].v_idx;
        const unsigned int vert_index3 = attrib.faces[3 * i + 2].v_idx;

        const unsigned int normal_index1 = attrib.faces[3 * i + 0].vn_idx;
        const unsigned int normal_index2 = attrib.faces[3 * i + 1].vn_idx;
        const unsigned int normal_index3 = attrib.faces[3 * i + 2].vn_idx;

        index_data[3 * i + 0] = vert_index1;
        index_data[3 * i + 1] = vert_index2;
        index_data[3 * i + 2] = vert_index3;

        // first f value
        vertex_data[(vert_index1 * 6) + 0] = attrib.vertices[vert_index1 * 3 + 0];
        vertex_data[(vert_index1 * 6) + 1] = attrib.vertices[vert_index1 * 3 + 1];
        vertex_data[(vert_index1 * 6) + 2] = attrib.vertices[vert_index1 * 3 + 2];
        vertex_data[(vert_index1 * 6) + 3] = attrib.normals[normal_index1 * 3 + 0];
        vertex_data[(vert_index1 * 6) + 4] = attrib.normals[normal_index1 * 3 + 1];
        vertex_data[(vert_index1 * 6) + 5] = attrib.normals[normal_index1 * 3 + 2];

        // second f value
        vertex_data[(vert_index2 * 6) + 0] = attrib.vertices[vert_index2 * 3 + 0];
        vertex_data[(vert_index2 * 6) + 1] = attrib.vertices[vert_index2 * 3 + 1];
        vertex_data[(vert_index2 * 6) + 2] = attrib.vertices[vert_index2 * 3 + 2];
        vertex_data[(vert_index2 * 6) + 3] = attrib.normals[normal_index2 * 3 + 0];
        vertex_data[(vert_index2 * 6) + 4] = attrib.normals[normal_index2 * 3 + 1];
        vertex_data[(vert_index2 * 6) + 5] = attrib.normals[normal_index2 * 3 + 2];

        // third f value
        vertex_data[(vert_index3 * 6) + 0] = attrib.vertices[vert_index3 * 3 + 0];
        vertex_data[(vert_index3 * 6) + 1] = attrib.vertices[vert_index3 * 3 + 1];
        vertex_data[(vert_index3 * 6) + 2] = attrib.vertices[vert_index3 * 3 + 2];
        vertex_data[(vert_index3 * 6) + 3] = attrib.normals[normal_index3 * 3 + 0];
        vertex_data[(vert_index3 * 6) + 4] = attrib.normals[normal_index3 * 3 + 1];
        vertex_data[(vert_index3 * 6) + 5] = attrib.normals[normal_index3 * 3 + 2];
    }

    for (size_t i = 0; i < attrib.num_vertices; i++)
    {
        printf("vert  [%lld] {%f, %f, %f} {%f, %f, %f}\n", i, vertex_data[i * 6 + 0], vertex_data[i * 6 + 1], vertex_data[i * 6 + 2],
               vertex_data[i * 6 + 3], vertex_data[i * 6 + 4], vertex_data[i * 6 + 5]);
    }

    for (size_t i = 0; i < m.num_indicies; i += 1)
    {
        printf("index [%lld] {%d}\n", i, index_data[i]);
    }

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

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, index_data_size, (const GLvoid *)index_data);

    VAO_Attr(vao, vbo_verticies, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(0));
    //VAO_Attr(vao, vbo_verticies, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    vec3 model_position = {0.0f, 0.0f, -2.0f};
    mat4 model_transform;
    glm_translate_make(model_transform, model_position);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "model", model_transform);

    m.shader = shader;

    VBO_Unbind();
    VAO_Unbind();

    fprintf(stderr, "Loading model complete : %s\n", file_path);

    return m;
}
