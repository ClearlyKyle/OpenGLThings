#include "Parallax_Mapping.h"

static struct ParallaxMapping
{
    Camera_t cam;

    FBO_t post_processing_fbo;
    FBO_t msaa_fbo;

    Shader_t shader_program;
    Shader_t shader_framebuffer;

    VAO_t     plane_VAO;
    Texture_t plane_tex_DIFFUSE;
    Texture_t plane_tex_SPECULAR;
    Texture_t plane_tex_NORMAL;
    Texture_t plane_tex_DISP;
    Shader_t  plane_SHADER;

    Shader_t light_Shader;
    VAO_t    light_VAO;
} pm;

void ParallaxMapping_Init()
{
    glEnable(GL_DEPTH_TEST);  // Enables the Depth Buffer
    glEnable(GL_MULTISAMPLE); // Enables Multisampling

    // glEnable(GL_CULL_FACE); // Enables Cull Facing
    // glCullFace(GL_FRONT);   // Keeps front faces
    // glFrontFace(GL_CCW);    // Uses counter clock-wise standard

    const GLfloat rectangle_vertices[] =
        {
            //  Coords   // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

    const struct Shader shader_program = Shader_Create2("../../Examples/24 - Parallax Occlusion Mapping/default.vs",
                                                        "../../Examples/24 - Parallax Occlusion Mapping/default.fs",
                                                        "../../Examples/24 - Parallax Occlusion Mapping/default.gs",
                                                        2,
                                                        (struct VertexAttribute[]){
                                                            {.index = 0, .name = "aPos"},
                                                            {.index = 1, .name = "aNormal"},
                                                            {.index = 2, .name = "aColor"},
                                                            {.index = 3, .name = "aTex"},
                                                        });

    const struct Shader shader_framebuffer = Shader_Create("../../Examples/24 - Parallax Occlusion Mapping/framebuffer.vs",
                                                           "../../Examples/24 - Parallax Occlusion Mapping/framebuffer.fs",
                                                           1,
                                                           (struct VertexAttribute[]){
                                                               {.index = 0, .name = "aPos"}});

    pm.shader_program     = shader_program;
    pm.shader_framebuffer = shader_framebuffer;

    const float gamma = 2.2f;

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);
    Shader_Uniform_Float(shader_framebuffer, "gamma", gamma);

    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};

    const Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    pm.cam             = cam;

    const VAO_t rect_VAO = VAO_Create();
    const VBO_t rect_VBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(rect_VBO, sizeof(rectangle_vertices), (const GLvoid *)rectangle_vertices);
    VAO_Attr(rect_VAO, rect_VBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(rect_VAO, rect_VBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    // MSAA FBO
    FBO_t msaa_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D_MULTISAMPLE, window.width, window.heigh, 4);
    FBO_Add_RBO(&msaa_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    msaa_fbo.VAO = rect_VAO.ID;
    pm.msaa_fbo  = msaa_fbo;

    // POST PROCESSING FBO
    FBO_t post_processing_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh, 0);
    post_processing_fbo.VAO   = rect_VAO.ID;
    pm.post_processing_fbo    = post_processing_fbo;

    { // SETUP PLANE
        const struct Shader shader = Shader_Create(
            "../../Examples/24 - Parallax Occlusion Mapping/texture_example.vs",
            "../../Examples/24 - Parallax Occlusion Mapping/texture_example.fs",
            4, (struct VertexAttribute[]){
                   {.index = 0, .name = "aPos"},
                   {.index = 1, .name = "aNormal"},
                   {.index = 2, .name = "aColor"},
                   {.index = 3, .name = "aTex"},
               });

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

        const struct VAO VAO = VAO_Create();
        const struct VBO VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(VBO, sizeof(plane_vertices), (const GLvoid *)plane_vertices);

        const struct EBO ebo = EBO_Create();
        EBO_Buffer(ebo, sizeof(plane_indicies), (const GLvoid *)plane_indicies);

        VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)0);
        VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 2, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 3, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(9 * sizeof(GLfloat)));

        const char    *diffuse_file_path      = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_DIFFUSE.png";
        const char    *specular_file_path     = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_SPECULAR.png";
        const char    *normal_file_path       = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_NORMAL.png";
        const char    *displacement_file_path = "../../Examples/res/textures/brick_wall/Tileable_Red_brick_texture_DISP.png";
        struct Texture diff_tex               = Texture_Create(diffuse_file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
        struct Texture spec_tex               = Texture_Create(specular_file_path, GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
        struct Texture nrm_tex                = Texture_Create(normal_file_path, GL_TEXTURE_2D, 2, GL_RGB, GL_UNSIGNED_BYTE);
        struct Texture disp_tex               = Texture_Create(displacement_file_path, GL_TEXTURE_2D, 3, GL_RED, GL_UNSIGNED_BYTE);

        vec3   translation = {0.0f, 0.0f, 0.0f};
        versor rotation    = {0.0f, 0.0f, 0.0f, 0.0f};
        vec3   scale       = {1.0f, 1.0f, 1.0f};

        mat4 object_translation = {0};
        mat4 object_roataion    = {0};
        mat4 object_scale       = {0};
        mat4 object_matrix      = GLM_MAT4_IDENTITY_INIT;

        glm_translate_make(object_translation, translation);
        glm_quat_mat4(rotation, object_roataion);
        glm_scale_make(object_scale, scale);

        Shader_Bind(shader);
        Shader_Uniform_Mat4(shader, "translation", object_translation);
        Shader_Uniform_Mat4(shader, "rotation", object_roataion);
        Shader_Uniform_Mat4(shader, "scale", object_scale);
        Shader_Uniform_Mat4(shader, "model", object_matrix);
        Shader_Uniform_Int(shader, "tex0", 0);

        Shader_Bind(shader_program);
        Shader_Uniform_Vec4(shader_program, "lightColor", light_colour);
        Shader_Uniform_Vec3(shader_program, "lightPos", light_position);
        Shader_Uniform_Mat4(shader_program, "translation", object_translation);
        Shader_Uniform_Mat4(shader_program, "rotation", object_roataion);
        Shader_Uniform_Mat4(shader_program, "scale", object_scale);
        Shader_Uniform_Mat4(shader_program, "model", object_matrix);
        Shader_Uniform_Int(shader_program, "diffuse0", 0);
        Shader_Uniform_Int(shader_program, "specular0", 1);
        Shader_Uniform_Int(shader_program, "normal0", 2);
        Shader_Uniform_Int(shader_program, "displacement0", 3);

        pm.plane_VAO          = VAO;
        pm.plane_tex_DIFFUSE  = diff_tex;
        pm.plane_tex_SPECULAR = spec_tex;
        pm.plane_tex_NORMAL   = nrm_tex;
        pm.plane_tex_DISP     = disp_tex;
        pm.plane_SHADER       = shader_program;
    } // SETUP PLANE

    { // LIGHT CUBE
        // Drawing a Cube to represent the Light source

        const GLfloat light_vertices[] =
            {//     COORDINATES     //
             -0.1f, -0.1f, 0.1f,
             -0.1f, -0.1f, -0.1f,
             0.1f, -0.1f, -0.1f,
             0.1f, -0.1f, 0.1f,
             -0.1f, 0.1f, 0.1f,
             -0.1f, 0.1f, -0.1f,
             0.1f, 0.1f, -0.1f,
             0.1f, 0.1f, 0.1f};

        const GLuint light_indices[] =
            {
                // front face
                0, 1, 2, 2, 3, 0,
                // top face
                3, 2, 6, 6, 7, 3,
                // back face
                7, 6, 5, 5, 4, 7,
                // left face
                4, 0, 3, 3, 7, 4,
                // bottom face
                0, 4, 5, 5, 1, 0, // Back to front now.
                                  // right face
                1, 5, 6, 6, 2, 1};

        struct Shader light_shader = Shader_Create(
            "../../Examples/18 - Blinn-Phong/lights.vs",
            "../../Examples/18 - Blinn-Phong/lights.fs",
            1,
            (struct VertexAttribute[]){
                {.index = 0, .name = "aPos"}});

        struct VAO light_vao = VAO_Create();

        struct EBO light_ebo = EBO_Create();
        EBO_Buffer(light_ebo, sizeof(light_indices), (void *)light_indices);

        struct VBO light_abo = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(light_abo, sizeof(light_vertices), (const GLvoid *)light_vertices);
        VAO_Attr(light_vao, light_abo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (const GLvoid *)(0));

        mat4 light_model = GLM_MAT4_ZERO_INIT;
        glm_translate_make(light_model, light_position);

        Shader_Bind(light_shader);
        Shader_Uniform_Mat4(light_shader, "model", light_model);
        Shader_Uniform_Vec4(light_shader, "lightColor", light_colour);

        pm.light_Shader = light_shader;
        pm.light_VAO    = light_vao;
    } // LIGHT CUBE
}

void ParallaxMapping_Update()
{
    Camera_Inputs(&pm.cam);

    Shader_Bind(pm.plane_SHADER);
    Shader_Uniform_Vec3(pm.plane_SHADER, "camPos", pm.cam.position);
    Camera_View_Projection_To_Shader(pm.cam, pm.plane_SHADER, "camMatrix");

    Texture_Bind(pm.plane_tex_DIFFUSE);
    Texture_Bind(pm.plane_tex_SPECULAR);
    Texture_Bind(pm.plane_tex_NORMAL);
    Texture_Bind(pm.plane_tex_DISP);

    VAO_Bind(pm.plane_VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //  Draw a Cube to represent the Light source
    Shader_Bind(pm.light_Shader);
    Camera_View_Projection_To_Shader(pm.cam, pm.light_Shader, "camMatrix");
    VAO_Bind(pm.light_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void ParallaxMapping_OnExit()
{
    Camera_Print_Values(pm.cam);

    Framebuffer_Destroy(&pm.post_processing_fbo);
    Framebuffer_Destroy(&pm.msaa_fbo);

    Shader_Destroy(&pm.shader_program);
    Shader_Destroy(&pm.shader_framebuffer);

    VAO_Destroy(pm.plane_VAO);
    Texture_Delete(pm.plane_tex_DIFFUSE);
    Texture_Delete(pm.plane_tex_SPECULAR);
    Texture_Delete(pm.plane_tex_NORMAL);
    Texture_Delete(pm.plane_tex_DISP);
    Shader_Destroy(&pm.plane_SHADER);

    VAO_Destroy(pm.light_VAO);
    Shader_Destroy(&pm.light_Shader);
}
