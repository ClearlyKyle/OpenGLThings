#include "ModelLoader.h"

#include "assimp_material.h"

// TINY OBJ
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj/tinyobj_loader_c.h"

static void _Load_File(void *ctx, const char *filename, const int is_mtl, const char *obj_filename, char **buffer, size_t *len)
{
    size_t  string_size = 0, read_size = 0;
    FILE   *handler;
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
        *buffer                = (char *)malloc(sizeof(char) * (string_size + 1));
        read_size              = fread(*buffer, sizeof(char), (size_t)string_size, handler);
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

    tinyobj_shape_t    *shape    = NULL;
    tinyobj_material_t *material = NULL;
    tinyobj_attrib_t    attrib;

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

    m.num_indicies     = attrib.num_face_num_verts * 3;
    m.num_of_verticies = m.num_indicies;

    // VERTICIES
    const GLsizeiptr vertex_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat         *vertex_data      = (GLfloat *)malloc(vertex_data_size);

    // NORMALS
    const GLsizeiptr normal_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat         *normal_data      = (GLfloat *)malloc(normal_data_size);

    // TEXTURES
    const GLsizeiptr texture_data_size = sizeof(GLfloat) * ((m.num_indicies) * 2);
    GLfloat         *texture_data      = (GLfloat *)malloc(texture_data_size);

    const GLsizeiptr index_data_size = sizeof(unsigned int) * (m.num_indicies);
    unsigned int    *index_data      = (unsigned int *)malloc(index_data_size);

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
    const char    *texture_file_path = "../../Examples/res/models/Dog House/Doghouse_PBR_BaseColor.png";
    struct Texture tex               = Texture_Create(texture_file_path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    m.tex                            = tex;

    Shader_Uniform_Texture2D(shader, "tex0", tex);

    m.shader = shader;
    VBO_Unbind();
    VAO_Unbind();

    fprintf(stderr, "Loading model complete : %s\n", file_path);

    return m;
}

struct Model Model_Import_Shader(const char *file_path, struct Shader shader)
{
    struct Model m;

    fprintf(stderr, "Loading mode : %s\n", file_path);

    tinyobj_shape_t    *shape    = NULL;
    tinyobj_material_t *material = NULL;
    tinyobj_attrib_t    attrib;

    size_t num_shapes;
    size_t num_materials;

    tinyobj_attrib_init(&attrib);

    const int ret = tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, file_path, _Load_File, NULL, 0);
    if (ret != TINYOBJ_SUCCESS)
    {
        fprintf(stderr, "ERROR!\n");
        exit(1);
    }

    fprintf(stderr, "num_shapes    : %d\n", (int)num_shapes);
    fprintf(stderr, "num_materials : %d\n", (int)num_materials);

    fprintf(stderr, "name        : %s\n", shape->name);
    fprintf(stderr, "face_offset : %d\n", shape->face_offset);
    fprintf(stderr, "length      : %d\n", shape->length);

    for (int i = 0; i < num_shapes; i++)
    {
        fprintf(stderr, "shape[%d] name = %s\n", i, shape[i].name);
    }

    _Print_TinyObj_Info(material, &attrib);

    m.num_indicies     = attrib.num_face_num_verts * 3;
    m.num_of_verticies = m.num_indicies;

    // VERTICIES
    const GLsizeiptr vertex_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat         *vertex_data      = (GLfloat *)malloc(vertex_data_size);

    // NORMALS
    const GLsizeiptr normal_data_size = sizeof(GLfloat) * ((m.num_indicies) * 3);
    GLfloat         *normal_data      = (GLfloat *)malloc(normal_data_size);

    // TEXTURES
    const GLsizeiptr texture_data_size = sizeof(GLfloat) * ((m.num_indicies) * 2);
    GLfloat         *texture_data      = (GLfloat *)malloc(texture_data_size);

    const GLsizeiptr index_data_size = sizeof(unsigned int) * (m.num_indicies);
    unsigned int    *index_data      = (unsigned int *)malloc(index_data_size);

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

    vec3 model_position = {0.0f, 0.0f, 0.0f};
    mat4 model_transform;
    glm_translate_make(model_transform, model_position);

    Shader_Bind(shader);
    Shader_Uniform_Mat4(shader, "model", model_transform);

    // TEXTURES
    const char *texture_name   = "Doghouse_PBR_BaseColor.png";
    const char *base_file_path = "../../Examples/res/models/Dog House/";
    char        texture_file_path[256];
    sprintf(texture_file_path, "%s%s", base_file_path, texture_name);

    struct Texture tex = Texture_Create(texture_file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    m.tex              = tex;

    Shader_Uniform_Texture2D(shader, "tex0", tex);

    m.shader = shader;
    VBO_Unbind();
    VAO_Unbind();

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shape, num_shapes);
    tinyobj_materials_free(material, num_materials);

    fprintf(stderr, "Loading model complete : %s\n", file_path);

    return m;
}

void _Load_Textures(const struct aiScene *scene, struct Mesh *mesh, const char *base_folder_path)
{
    /* scan scene's materials for textures */
    struct TextureInfo texInfo = {0};

    texInfo.tex_count = 0;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        // if (tex_count == 0)
        //     continue;

        struct aiString path; // filename
        // struct Texture tex;

        // fprintf(stderr, "Searching for all texture files...\n");
        for (int texture_type = 0; texture_type <= aiTextureType_UNKNOWN; texture_type++)
        {
            const unsigned int texture_type_count = aiGetMaterialTextureCount(scene->mMaterials[i], texture_type);
            // printf("%d %s texture\n", texture_type_count, TextureTypeToString(texture_type));

            int texIndex = 0;
            while (aiGetMaterialTexture(scene->mMaterials[i], texture_type, texIndex, &path, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                texIndex++;

                fprintf(stderr, "(i = %d)(texIndex : %d)(type = %d) aiGetMaterialTexture [%d]: %s\n", i, texIndex, texture_type, texIndex, path.data);

                // TODO : Search for texture function?
                bool save_this_path = true;
                for (size_t i = 1; i <= texInfo.tex_count; i++)
                {
                    // if strings are equal, the function returns 0
                    if (strcmp(texInfo.texture_file_paths[texInfo.tex_count - 1], path.data) == 0)
                        save_this_path = false;
                }
                if (save_this_path == true)
                {
                    // TODO : Create and bind texture here?
                    texInfo.texture_file_paths[texInfo.tex_count] = (char *)malloc(sizeof(char) * (path.length));
                    strcpy(texInfo.texture_file_paths[texInfo.tex_count], path.data);
                    texInfo.texture_type[texInfo.tex_count] = texture_type;

                    texInfo.tex_count += 1;
                }
            }
        }
    }

    if (texInfo.tex_count == 0)
    {
        mesh->texInfo = texInfo;
        return;
    }

    fprintf(stderr, "Textures found...\n");
    for (size_t i = 0; i < texInfo.tex_count; i++)
    {
        fprintf(stderr, "[%lld] %s\n", i, texInfo.texture_file_paths[i]);
    }

    // size_t counter = 0;
    // for (char *p = texInfo.texture_file_paths, *end = texInfo.texture_file_paths[texInfo.tex_count]; p != end; p++, counter++)
    //{
    //     fprintf(stderr, "[%lld] %s\n", counter, *p);
    // }

    texInfo.textures = (struct Texture *)malloc(sizeof(struct Texture) * texInfo.tex_count);

    Shader_Bind(mesh->shader);
    for (size_t i = 0; i <= texInfo.tex_count - 1; i++)
    {
        char buff[256];
        sprintf(buff, "%s%s", base_folder_path, texInfo.texture_file_paths[i]);
        // strcat(base_file_path, mesh->tex_names[i]);

        const char *texture_type_str = TextureTypeToString(texInfo.texture_type[i]);

        switch (texInfo.texture_type[i])
        {
        case aiTextureType_DIFFUSE:
        {
            struct Texture tex  = Texture_Create(buff, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
            texInfo.textures[i] = tex;
            Shader_Uniform_Texture2D(mesh->shader, "diffuse0", tex);
            fprintf(stderr, "[slot - %d](type: %s) %s\n", 0, "diffuse0", buff);
            break;
        }
        case aiTextureType_SPECULAR:
        {
            struct Texture tex  = Texture_Create(buff, GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);
            texInfo.textures[i] = tex;
            Shader_Uniform_Texture2D(mesh->shader, "specular0", tex);
            fprintf(stderr, "[slot - %d](type: %s) %s\n", 1, "specular0", buff);
            break;
        }
        case aiTextureType_NORMALS:
        {
            struct Texture tex  = Texture_Create(buff, GL_TEXTURE_2D, 2, GL_RGBA, GL_UNSIGNED_BYTE);
            texInfo.textures[i] = tex;
            Shader_Uniform_Texture2D(mesh->shader, "normal0", tex);
            fprintf(stderr, "[slot - %d](type: %s) %s\n", 2, "normal0", buff);
            break;
        }

        default:
            break;
        }
    }

    mesh->texInfo = texInfo;

    // TODO : Cleanup
    fprintf(stderr, "Textures Laoded\n");
}

static void _aiColor4D_to_glm_vec4(struct aiColor4D col, vec4 glm_col)
{
    glm_col[0] = col.r;
    glm_col[1] = col.g;
    glm_col[2] = col.b;
    glm_col[3] = col.a;
}

static void _Get_Path_To_File(const char *full_path, char *path_to_file, char sperator)
{
    char        *last          = strrchr(full_path, sperator);
    const size_t full_path_len = strlen(full_path);

    if (last != NULL)
    {
        const size_t parentLen = full_path_len - strlen(last + 1);
        strncpy(path_to_file, full_path, parentLen);
        path_to_file[parentLen] = '\0';
    }
}

static struct Mesh _Mesh_Load_Data(const struct Shader shader, unsigned int instance_count, mat4 *matrix, const char *file_path)
{
    struct Mesh my_mesh = {
        .matrix      = GLM_MAT4_IDENTITY_INIT,
        .translation = {0.0f, 0.0f, 0.0f},
        .rotation    = {0.0f, 0.0f, 0.0f, 0.0f},
        .scale       = {1.0f, 1.0f, 1.0f}};

    my_mesh.shader            = shader;
    my_mesh.instancing.count  = instance_count;
    my_mesh.instancing.matrix = NULL;

    // struct aiScene *scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Quality);
    struct aiScene *scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder);
    if (!scene) // If the import failed, report it
    {
        fprintf(stderr, "[MODEL] Error with 'aiImportFile'\n%s\n", aiGetErrorString());
        exit(1);
    }
    // Now we can access the file's contents.
    fprintf(stderr, "[MODEL] Sucessfil Import of scene file : %s\n", file_path);

    my_mesh.scene = scene;

    char base_folder_path[256];
    _Get_Path_To_File(file_path, base_folder_path, '/');
    printf("base_folder_path : %s\n", base_folder_path);
    _Load_Textures(scene, &my_mesh, base_folder_path);

    // For each mesh
    my_mesh.num_models = scene->mNumMeshes;
    my_mesh.models     = (struct Model *)malloc(sizeof(struct Model) * scene->mNumMeshes);

    my_mesh.material_ubo_index = 0;
    my_mesh.ubo_size           = sizeof(struct MaterialInfo);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        struct MaterialInfo material_info = {0};
        struct Model        model;

        const struct aiMesh *mesh = scene->mMeshes[i];

        // Convert Assimp faces format to array format
        unsigned int *faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
        unsigned int  faceIndex = 0;

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const struct aiFace *face = &mesh->mFaces[f];

            memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
            faceIndex += 3;
        }
        model.num_indicies = scene->mMeshes[i]->mNumFaces;

        // fprintf(stderr, "mMaterialIndex : %d\n", mesh->mMaterialIndex);

        // generate Vertex Array for mesh
        struct VAO vao = VAO_Create(); // Bound
        model.vao      = vao;

        // buffer for faces
        struct EBO ebo_faces = EBO_Create(); // Bound
        EBO_Buffer(ebo_faces, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray);

        // buffer for vertex positions
        if (mesh->mVertices != NULL)
        {
            struct VBO verticies = VBO_Create(GL_ARRAY_BUFFER);
            VBO_Buffer(verticies, sizeof(float) * 3 * mesh->mNumVertices, (const GLvoid *)mesh->mVertices);
            VAO_Attr(vao, verticies, 0, 3, GL_FLOAT, 3 * sizeof(float), (const GLvoid *)(0));
        }

        // buffer for vertex normals
        if (mesh->mNormals != NULL)
        {
            struct VBO normals = VBO_Create(GL_ARRAY_BUFFER);
            VBO_Buffer(normals, sizeof(float) * 3 * mesh->mNumVertices, (const GLvoid *)mesh->mNormals);
            VAO_Attr(vao, normals, 1, 3, GL_FLOAT, 3 * sizeof(float), (const GLvoid *)(0));
        }

        // buffer for vertex colours
        const size_t number_of_colours = sizeof(mesh->mColors) / sizeof(mesh->mColors[0]);
        if (mesh->mColors[0])
        {
            struct VBO colours = VBO_Create(GL_ARRAY_BUFFER);
            // VBO_Buffer(colours, sizeof(float) * 3 * mesh->mNumVertices, (const GLvoid *)mesh->mColors);

            float *colourArray = (float *)malloc(sizeof(float) * mesh->mNumFaces * 4);
            for (size_t c = 0, colour_index = 0; c < number_of_colours; c++, colour_index += 4)
            {
                struct aiColor4D *const *colour = &mesh->mColors[c];

                memcpy(&colourArray[colour_index], colour, 4 * sizeof(float));
            }
            VBO_Buffer(colours, sizeof(float) * 4 * number_of_colours, (const GLvoid *)colourArray);
            VAO_Attr(vao, colours, 3, 4, GL_FLOAT, 4 * sizeof(float), (const GLvoid *)(0));
        }

        // buffer for vertex texture coordinates
        if (mesh->mTextureCoords[0] != NULL)
        {
            float *texCoords = (float *)malloc(sizeof(float) * 2 * mesh->mNumVertices);
            for (unsigned int k = 0; k < mesh->mNumVertices; k++)
            {
                texCoords[k * 2 + 0] = mesh->mTextureCoords[0][k].x;
                texCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;
            }
            struct VBO tex_coordinates = VBO_Create(GL_ARRAY_BUFFER);
            VBO_Buffer(tex_coordinates, sizeof(float) * 2 * mesh->mNumVertices, (const GLvoid *)texCoords);
            VAO_Attr(vao, tex_coordinates, 2, 2, GL_FLOAT, 2 * sizeof(float), (const GLvoid *)(0));
        }

        // Instanced matrix
        if (my_mesh.instancing.count > 1 && matrix != NULL)
        {
            struct VBO instance_VBO = VBO_Create(GL_ARRAY_BUFFER);
            VBO_Buffer(instance_VBO, instance_count * sizeof(mat4), (const GLvoid *)matrix);

            // Can't link to a mat4 so you need to link four vec4s
            VAO_Attr(vao, instance_VBO, 4, 4, GL_FLOAT, sizeof(mat4), (const GLvoid *)0);
            VAO_Attr(vao, instance_VBO, 5, 4, GL_FLOAT, sizeof(mat4), (const GLvoid *)(1 * sizeof(vec4)));
            VAO_Attr(vao, instance_VBO, 6, 4, GL_FLOAT, sizeof(mat4), (const GLvoid *)(2 * sizeof(vec4)));
            VAO_Attr(vao, instance_VBO, 7, 4, GL_FLOAT, sizeof(mat4), (const GLvoid *)(3 * sizeof(vec4)));

            // Makes it so the transform is only switched when drawing the next instance
            VAO_Bind(vao); // DONT FORGET TO BIND VAO BEFORE glVertexAttribDivisor
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
        }

        // unbind buffers
        VAO_Unbind();
        VBO_Unbind();

        // create material uniform buffer
        struct aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

        // Look for textures
        struct aiString     texPath; // contains filename of texture
        const enum aiReturn ret = aiGetMaterialTexture(mtl, aiTextureType_DIFFUSE, 0, &texPath, NULL, NULL, NULL, NULL, NULL, NULL);
        if (ret == AI_SUCCESS)
        {
            for (size_t i = 0; i < my_mesh.texInfo.tex_count; i++)
            {
                if (strcmp(my_mesh.texInfo.texture_file_paths[i], texPath.data) == 0)
                {
                    model.tex = my_mesh.texInfo.textures[i];
                    break;
                }
            }
        }
        else
        {
            // TODO : Mesh does not use a texture..
            fprintf(stderr, "%s does not use a texture..\n", mesh->mName.data);
        }

        // AMBIENT
        struct aiColor4D ambient = {0.2f, 0.2f, 0.2f, 1.0f};
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
        {
            _aiColor4D_to_glm_vec4(ambient, material_info.ambient);
        }

        // DIFFUSE
        struct aiColor4D diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        {
            _aiColor4D_to_glm_vec4(diffuse, material_info.diffuse);
        }

        // SPECULAR
        struct aiColor4D specular = {0.0f, 0.0f, 0.0f, 1.0f};
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
        {
            _aiColor4D_to_glm_vec4(specular, material_info.specular);
        }

        // EMISSION
        struct aiColor4D emission = {0.0f, 0.0f, 0.0f, 1.0f};
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
        {
            _aiColor4D_to_glm_vec4(emission, material_info.emission);
        }

        float        shininess = 0.0f;
        unsigned int max;
        if (AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max))
        {
            material_info.shininess = shininess;
        }

        struct VBO materials = VBO_Create(GL_UNIFORM_BUFFER);
        VBO_Buffer(materials, sizeof(struct MaterialInfo), (const GLvoid *)&material_info);
        model.material_vbo = materials;

        model.material_info = material_info;
        my_mesh.models[i]   = model;
    }

    fprintf(stderr, "[MODEL] Mesh loading complete!\n\n");
    return my_mesh;
}

Mesh_t Mesh_Load_Instancing(const struct Shader shader, GLuint count, mat4 *matrix, const char *file_path)
{
    check_that((count > 1), "Why are you using this with only one instance?\n");

    fprintf(stderr, "[MODEL INSTANCING] Loading instancing!\n\n");

    Mesh_t mesh = _Mesh_Load_Data(shader, count, matrix, file_path);

    fprintf(stderr, "[MODEL INSTANCING] instancing ready to use...\n\n");
    return mesh;
}

struct Mesh Mesh_Load(const struct Shader shader, const char *file_path)
{
    return _Mesh_Load_Data(shader, 1, NULL, file_path);
}

void Mesh_Instance_Load_Vectors(Mesh_t *mesh, GLuint index, vec3 trans, versor rot, vec3 scale)
{
    // TODO : Move this calculation to the shader?

    // Initialize matrices
    mat4 mat_trans = GLM_MAT4_IDENTITY_INIT;
    mat4 mat_rot   = GLM_MAT4_IDENTITY_INIT;
    mat4 mat_scale = GLM_MAT4_IDENTITY_INIT;

    // Transform the matrices to their correct form
    glm_translate_make(mat_trans, trans);
    glm_quat_mat4(rot, mat_rot);
    glm_scale_make(mat_scale, scale);

    // trans * rot * sca
    mat4 matrix_res = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_mul(mat_trans, mat_rot, matrix_res);
    glm_mat4_mul(matrix_res, mat_scale, matrix_res);

    glm_mat4_copy(matrix_res, mesh->instancing.matrix[index]);
}

static void _Recursive_Mesh_Renderer(struct Mesh m, const struct aiScene *sc, const struct aiNode *nd)
{
    // Get node transformation matrix
    // OpenGL matrices are column major
    struct aiMatrix4x4 mTrans = nd->mTransformation;
    float              aux[16];
    memcpy(aux, &mTrans, sizeof(float) * 16);

    // draw all meshes assigned to this node
    for (unsigned int n = 0; n < nd->mNumMeshes; n++)
    {
        // bind material uniform
        UBO_Bind_Buffer_To_Index(m.models[nd->mMeshes[n]].material_vbo.ID, 0, 0, sizeof(struct MaterialInfo));

        if (m.models[nd->mMeshes[n]].tex.ID < 100)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(m.models[nd->mMeshes[n]].tex.type, m.models[nd->mMeshes[n]].tex.ID);
        }

        VAO_Bind(m.models[nd->mMeshes[n]].vao);

        // https://stackoverflow.com/questions/40202354/glsl-shader-to-support-coloring-and-texturing

        if (m.instancing.count > 1)
        {
            glDrawElementsInstanced(GL_TRIANGLES, m.models[nd->mMeshes[n]].num_indicies * 3, GL_UNSIGNED_INT, 0, m.instancing.count);
        }
        else
        {
            // NORMAL NON INSTANCED DRAWING
            // Update trans, rot and scale
            // mat4 mat_trans = GLM_MAT4_ZERO_INIT;
            // mat4 mat_rot   = GLM_MAT4_ZERO_INIT;
            // mat4 mat_scale = GLM_MAT4_ZERO_INIT;

            // glm_translate_make(mat_trans, m.translation);
            // glm_quat_mat4(m.rotation, mat_rot);
            // glm_scale_make(mat_scale, m.scale);

            // uniform mat4 model;
            // uniform mat4 translation;
            // uniform mat4 rotation;
            // uniform mat4 scale;

            // Send to shader uniforms
            Shader_Uniform_Mat4_Floats(m.shader, "translation", aux);
            // Shader_Uniform_Mat4(m.shader, "translation", mat_trans);
            // Shader_Uniform_Mat4(m.shader, "rotation", mat_rot);
            // Shader_Uniform_Mat4(m.shader, "scale", mat_scale);
            // Shader_Uniform_Mat4(m.shader, "model", m.matrix);

            glDrawElements(GL_TRIANGLES, m.models[nd->mMeshes[n]].num_indicies * 3, GL_UNSIGNED_INT, 0);
        }
        VAO_Unbind();
    }

    // draw all children
    for (unsigned int n = 0; n < nd->mNumChildren; n++)
    {
        _Recursive_Mesh_Renderer(m, sc, nd->mChildren[n]);
    }
}

void Mesh_Set_Translation(struct Mesh *mesh, vec3 new_translation)
{
    glm_vec3_copy(new_translation, mesh->translation);
}

void Mesh_Set_Rotation(struct Mesh *mesh, vec4 new_rotation)
{
    glm_vec4_copy(new_rotation, mesh->rotation);
}

void Mesh_Set_Scale(struct Mesh *mesh, vec3 new_scale)
{
    glm_vec3_copy(new_scale, mesh->scale);
}

// void Mesh_Draw(struct Mesh m, Camera_t cam)
void Mesh_Draw(struct Mesh m)
{
    Shader_Bind(m.shader);

    // Shader_Uniform_Vec3(m.shader, "camPos", cam.position);
    // Camera_View_Projection_To_Shader(cam, m.shader, "camMatrix");

    _Recursive_Mesh_Renderer(m, m.scene, m.scene->mRootNode);
}

// void Mesh_Draw_with_Shader(struct Mesh m, struct Shader shader)
//{
//     Shader_Bind(shader);
//     _Recursive_Mesh_Renderer(m, m.scene, m.scene->mRootNode);
// }

void Mesh_Draw_Shader(struct Mesh m, struct Shader shader, struct Camera cam)
{
    Shader_Bind(shader);

    Shader_Uniform_Vec3(shader, "camPos", cam.position);
    Camera_View_Projection_To_Shader(cam, shader, "camMatrix");

    m.shader = shader;
    _Recursive_Mesh_Renderer(m, m.scene, m.scene->mRootNode);
    //_Recursive_Mesh_Renderer(m, m.scene, m.scene->mRootNode, shader);
}

void Model_Free(struct Model model)
{
    Shader_Destroy(&model.shader);
    VAO_Destroy(model.vao);
}

void Mesh_Free(struct Mesh mesh)
{
    aiReleaseImport(mesh.scene);

    for (unsigned int i = 0; i < mesh.num_models; i++)
    {
        VAO_Destroy(mesh.models[i].vao);
        // Shader_Destroy(mesh.models[i].shader);
        Texture_Delete(mesh.models[i].tex);
    }

    Shader_Destroy(&mesh.shader);

    if (mesh.instancing.count > 1)
    {
        free(mesh.instancing.matrix);
    }

    free(mesh.models);
    mesh.models = NULL;
}