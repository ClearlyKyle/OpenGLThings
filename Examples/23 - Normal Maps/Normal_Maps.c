#include "Normal_Maps.h"

static struct NormalMaps
{
    Camera_t cam;

    FBO_t msaa_fbo;
    FBO_t post_processing_fbo;

    VAO_t     plane_VAO;
    Texture_t plane_TEXTURE;
    Shader_t  plane_SHADER;
} nm;

void NormalMaps_Init()
{
    const GLfloat rectangle_vertices[] =
        {
            //  Coords   // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

    // Vertices for plane with texture
    // const GLfloat plane_vertices[] =
    //    {
    //        -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    //        -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    //        1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    //        1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    // const GLfloat plane_indicies[] = {
    //     0, 1, 2,
    //     0, 2, 3};

    struct Shader shader_program = Shader_Create2("../../Examples/23 - Normal Maps/default.vs",
                                                  "../../Examples/23 - Normal Maps/default.fs",
                                                  "../../Examples/23 - Normal Maps/default.gs",
                                                  2,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "inPos"},
                                                      {.index = 1, .name = "inTexCoords"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/23 - Normal Maps/framebuffer.vs",
                                                     "../../Examples/23 - Normal Maps/framebuffer.fs",
                                                     1,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "aPos"}});

    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};

    const float gamma = 2.2f;

    Shader_Bind(shader_program);
    Shader_Uniform_Vec4(shader_program, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader_program, "lightPos", light_position);

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);
    Shader_Uniform_Float(shader_framebuffer, "gamma", gamma);

    glEnable(GL_DEPTH_TEST);  // Enables the Depth Buffer
    glEnable(GL_MULTISAMPLE); // Enables Multisampling

    // glEnable(GL_CULL_FACE); // Enables Cull Facing
    // glCullFace(GL_FRONT);   // Keeps front faces
    // glFrontFace(GL_CCW);    // Uses counter clock-wise standard

    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    nm.cam       = cam;

    VAO_t rect_VAO = VAO_Create();
    VBO_t rect_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(rect_VBO, sizeof(rectangle_vertices), (const GLvoid *)rectangle_vertices);
    VAO_Attr(rect_VAO, rect_VBO, 0, 4 * sizeof(GLfloat), GL_FLOAT, 2, (const GLvoid *)(0));
    VAO_Attr(rect_VAO, rect_VBO, 1, 4 * sizeof(GLfloat), GL_FLOAT, 2, (const GLvoid *)(2 * sizeof(GLfloat)));

    // MSAA FBO
    FBO_t msaa_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D_MULTISAMPLE, window.width, window.heigh, 4);
    FBO_Add_RBO(&msaa_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    msaa_fbo.VAO = rect_VAO.ID;
    nm.msaa_fbo  = msaa_fbo;

    // POST PROCESSING FBO
    FBO_t post_processing_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh, 0);
    post_processing_fbo.VAO   = rect_VAO.ID;
    nm.post_processing_fbo    = post_processing_fbo;

    // const Texture_t plane_textures[] = {
    //     Texture_Create("", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE),
    // };

    // MMesh_t plane = Mesh_Construct(plane_vertices, plane_indicies, plane_textures);

    { // SETUP PLANE
        const struct Shader shader = Shader_Create(
            "../../Examples/23 - Normal Maps/texture_example.vs",
            "../../Examples/23 - Normal Maps/texture_example.fs",
            4, (struct VertexAttribute[]){
                   {.index = 0, .name = "aPos"},
                   {.index = 1, .name = "aNormal"},
                   {.index = 2, .name = "aColor"},
                   {.index = 3, .name = "aTex"},
               });

        const GLfloat plane_vertices[] =
            {
                //     COORDINATES  / NORMALS /     COLORS      / TexCoord  //
                -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        const GLuint plane_indicies[] =
            {
                0, 2, 1, // Upper triangle
                0, 3, 2  // Lower triangle
            };
        struct VAO VAO = VAO_Create();
        struct VBO VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(VBO, sizeof(plane_vertices), (const GLvoid *)plane_vertices);

        struct EBO ebo = EBO_Create();
        EBO_Buffer(ebo, sizeof(plane_indicies), (const GLvoid *)plane_indicies);

        VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)0);
        VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 2, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 3, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(9 * sizeof(GLfloat)));

        const char    *file_path = "../../Examples/res/textures/diffuse.png";
        struct Texture tex       = Texture_Create(file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);

        vec3 object_position = {0.5f, 0.5f, 0.5f};
        mat4 object_model    = GLM_MAT4_ZERO_INIT;
        glm_translate_make(object_model, object_position);

        Shader_Bind(shader);
        Shader_Uniform_Mat4(shader, "model", object_model);
        Shader_Uniform_Int(shader, "tex0", 0);

        nm.plane_VAO     = VAO;
        nm.plane_TEXTURE = tex;
        nm.plane_SHADER  = shader;
    } // SETUP PLANE

    // Texture_t normal_map = Texture_Create("", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
}

void NormalMaps_Update()
{
    Camera_Inputs(&nm.cam);

    Shader_Bind(nm.plane_SHADER);
    Camera_View_Projection_To_Shader(nm.cam, nm.plane_SHADER, "camMatrix");

    Texture_Bind(nm.plane_TEXTURE);

    VAO_Bind(nm.plane_VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void NormalMaps_OnExit()
{
    Camera_Print_Values(nm.cam);

    Framebuffer_Destroy(&nm.msaa_fbo);
    Framebuffer_Destroy(&nm.post_processing_fbo);

    VAO_Destroy(nm.plane_VAO);
    Texture_Delete(nm.plane_TEXTURE);
    Shader_Destroy(&nm.plane_SHADER);
}
