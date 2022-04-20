#include "CubeBoxes.h"

#include "stb/stb_image.h"

static struct CubeBox
{
    struct Mesh   model;
    struct Camera cam;
    struct Shader sky_box_shader;

    GLuint skybox_VAO;
    GLuint cubemap_texture;
} cb;

const GLfloat skybox_verticies[] =
    {
        //   Coordinates
        -1.0f, -1.0f, 1.0f,  //        7--------6
        1.0f, -1.0f, 1.0f,   //       /|       /|
        1.0f, -1.0f, -1.0f,  //      4--------5 |
        -1.0f, -1.0f, -1.0f, //      | |      | |
        -1.0f, 1.0f, 1.0f,   //      | 3------|-2
        1.0f, 1.0f, 1.0f,    //      |/       |/
        1.0f, 1.0f, -1.0f,   //      0--------1
        -1.0f, 1.0f, -1.0f};

const GLuint skybox_indicies[] =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3};

void CubeBoxes_Init()
{
    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/14 - Cubemaps & Skyboxes/default.vs",
                                                 "../../Examples/14 - Cubemaps & Skyboxes/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aColor"},
                                                     {.index = 3, .name = "aTex"}});

    struct Shader sky_box_shader = Shader_Create("../../Examples/14 - Cubemaps & Skyboxes/skybox.vs",
                                                 "../../Examples/14 - Cubemaps & Skyboxes/skybox.fs",
                                                 1,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "inPos"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(sky_box_shader);
    glUniform1i(glGetUniformLocation(sky_box_shader.shader_id, "skybox"), 0);
    cb.sky_box_shader = sky_box_shader;

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);    // Keeps front faces
    glFrontFace(GL_CCW);     // Uses counter clock-wise standard

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){4.037022f, 0.606838f, -4.618166f}, cam.position);
    glm_vec3_copy((vec3){-0.679725f, -0.080201f, 0.729069f}, cam.orientation);
    cam.pitch = -4.600101f;
    cam.yaw   = -227.006027f;

    cb.cam = cam;

    // Loading Models
    struct Mesh airplane = Mesh_Load(shader_default, "../../Examples/res/models/airplane/scene.gltf");
    cb.model             = airplane;

    // Create VAO, VBO, and EBO for the skybox
    GLuint skybox_VAO, skybox_VBO, skybox_EBO;
    glGenVertexArrays(1, &skybox_VAO);
    glGenBuffers(1, &skybox_VBO);
    glGenBuffers(1, &skybox_EBO);
    glBindVertexArray(skybox_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_verticies), &skybox_verticies, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indicies), &skybox_indicies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    cb.skybox_VAO = skybox_VAO;

    // All the faces of the cubemap (make sure they are in this exact order)
    const char *cube_map_face_images[6] =
        {
            "../../Examples/res/textures/skybox/right.jpg",
            "../../Examples/res/textures/skybox/left.jpg",
            "../../Examples/res/textures/skybox/top.jpg",
            "../../Examples/res/textures/skybox/bottom.jpg",
            "../../Examples/res/textures/skybox/front.jpg",
            "../../Examples/res/textures/skybox/back.jpg"};

    // Creates the cubemap texture object
    GLuint cubemap_texture;
    glGenTextures(1, &cubemap_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // This might help with seams on some systems
    // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    cb.cubemap_texture = cubemap_texture;

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        int            width, height, bpp;
        unsigned char *image_data = stbi_load(cube_map_face_images[i], &width, &height, &bpp, 0);

        check_that(image_data, "Failed to load texture: %s\n", cube_map_face_images[i]);

        stbi_set_flip_vertically_on_load(false);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     image_data);

        stbi_image_free(image_data);
    }
}

void CubeBoxes_Update()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);            // Specify the color of the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the back buffer and depth buffer

    Camera_Inputs(&cb.cam);

    Shader_Uniform_Vec3(cb.model.shader, "camPos", cb.cam.position);
    Camera_View_Projection_To_Shader(cb.cam, cb.model.shader, "camMatrix");

    Mesh_Draw(cb.model); // Draw the airplane model

    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    Shader_Bind(cb.sky_box_shader);

    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection  = GLM_MAT4_IDENTITY_INIT;
    // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
    // The last row and column affect the translation of the skybox (which we don't want to affect)

    Camera_Get_View_Matrix(cb.cam, view_matrix);
    view_matrix[0][3]  = 0.0f;
    view_matrix[0][7]  = 0.0f;
    view_matrix[0][11] = 0.0f;
    view_matrix[0][15] = 0.0f;
    view_matrix[3][0]  = 0.0f;
    view_matrix[3][1]  = 0.0f;
    view_matrix[3][2]  = 0.0f;
    view_matrix[3][3]  = 0.0f;

    glm_perspective(glm_rad(45.0f), (float)window.width / window.heigh, 0.1f, 100.0f, projection);

    Shader_Uniform_Mat4(cb.sky_box_shader, "view", view_matrix);
    Shader_Uniform_Mat4(cb.sky_box_shader, "projection", projection);

    // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
    // where an object is present (a depth of 1.0f will always fail against any object's depth value)
    glBindVertexArray(cb.skybox_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cb.cubemap_texture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);
}

void CubeBoxes_OnExit()
{
    Camera_Print_Values(cb.cam);

    Mesh_Free(cb.model);
    Shader_Destroy(cb.sky_box_shader);

    glDeleteVertexArrays(1, &cb.skybox_VAO);
    glDeleteTextures(1, &cb.cubemap_texture);
}
