#include "Instancing.h"

#include <time.h>
#include "stb/stb_image.h"

#define NUMBER_OF_ASTEROIDS 500

static struct Instancing
{
    struct Mesh   jupiter;
    struct Mesh   asteroid;
    struct Camera cam;
    struct Shader sky_box_shader;

    GLuint skybox_VAO;
    GLuint cubemap_texture;

    // Holds all transformations for the asteroids
    vec3   translations[NUMBER_OF_ASTEROIDS];
    versor rotations[NUMBER_OF_ASTEROIDS];
    vec3   scales[NUMBER_OF_ASTEROIDS];

} ins;

static float randf()
{
    // return min + rand() / (float)RAND_MAX * (max - min);
    return (-1.0f) + (rand() / (float)RAND_MAX) * ((1.0f) - (-1.0f));
}

void Instancing_Init()
{
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

    const GLuint skybox_indices[] =
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

    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/16 - Instancing/default.vs",
                                                 "../../Examples/16 - Instancing/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader skybox_shader = Shader_Create("../../Examples/16 - Instancing/skybox.vs",
                                                "../../Examples/16 - Instancing/skybox.fs",
                                                1,
                                                (struct VertexAttribute[]){
                                                    {.index = 0, .name = "inPos"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(skybox_shader);
    glUniform1i(glGetUniformLocation(skybox_shader.shader_id, "skybox"), 0);
    ins.sky_box_shader = skybox_shader;

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);    // Keeps front faces
    glFrontFace(GL_CCW);     // Uses counter clock-wise standard

    // Create VAO, VBO, and EBO for the skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_verticies), &skybox_verticies, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indices), &skybox_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *)(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ins.skybox_VAO = skyboxVAO;

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
    ins.cubemap_texture = cubemap_texture;

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        stbi_set_flip_vertically_on_load(false);

        int            width, height, bpp;
        unsigned char *image_data = stbi_load(cube_map_face_images[i], &width, &height, &bpp, 0);

        check_that(image_data, "Failed to load texture: %s\n", cube_map_face_images[i]);

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

    // Load in models
    struct Mesh jupiter  = Mesh_Load(shader_default, "../../Examples/res/models/jupiter/scene.gltf");
    struct Mesh asteroid = Mesh_Load(shader_default, "../../Examples/res/models/asteroid/scene.gltf");
    ins.jupiter          = jupiter;
    ins.asteroid         = asteroid;

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-155.146210f, 60.443268f, -131.134857f}, cam.position);
    glm_vec3_copy((vec3){0.843958f, -0.381072f, 0.377516f}, cam.orientation);
    cam.pitch = -22.400122f;
    cam.yaw   = 24.099716f;
    ins.cam   = cam;

    // Setup ASTEROIDS
    const float radius           = 100.0f; // Radius of circle around which asteroids orbit
    const float radius_deviation = 25.0f;  // How much ateroids deviate from the radius

    srand((unsigned int)time(NULL));
    for (unsigned int i = 0; i < NUMBER_OF_ASTEROIDS; i++)
    {
        // Generates x and y for the function x^2 + y^2 = radius^2 which is a circle
        const float x           = randf();
        const float finalRadius = radius + randf() * radius_deviation;
        const float y           = ((rand() % 2) * 2 - 1) * (float)sqrt(1.0f - x * x);

        // Makes the random distribution more even
        if (randf() > 0.5f)
        {
            // Generates a translation near a circle of radius "radius"
            glm_vec3_copy((vec3){y * finalRadius, randf(), x * finalRadius}, ins.translations[i]);
        }
        else
        {
            // Generates a translation near a circle of radius "radius"
            glm_vec3_copy((vec3){x * finalRadius, randf(), y * finalRadius}, ins.translations[i]);
        }

        // Generates random rotations
        glm_vec4_copy((vec4){1.0f, randf(), randf(), randf()}, ins.rotations[i]);

        // Generates random scales
        float scale_value = (0.2f * randf());
        if (scale_value < 0.0f)
            scale_value *= -1.0f;
        glm_vec3_copy((vec3){scale_value, scale_value, scale_value}, ins.scales[i]);
    }
}

void Instancing_Update()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);            // Specify the color of the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the back buffer and depth buffer

    Camera_Inputs(&ins.cam);

    Shader_Uniform_Vec3(ins.jupiter.shader, "camPos", ins.cam.position);
    Camera_View_Projection_To_Shader(ins.cam, ins.jupiter.shader, "camMatrix");

    Mesh_Draw(ins.jupiter);

    // Draw the asteroids
    Shader_Uniform_Vec3(ins.asteroid.shader, "camPos", ins.cam.position);
    Camera_View_Projection_To_Shader(ins.cam, ins.asteroid.shader, "camMatrix");

    for (unsigned int i = 0; i < NUMBER_OF_ASTEROIDS; i++)
    {
        Mesh_Set_Translation(&ins.asteroid, ins.translations[i]);
        Mesh_Set_Rotation(&ins.asteroid, ins.rotations[i]);
        Mesh_Set_Scale(&ins.asteroid, ins.scales[i]);
        Mesh_Draw(ins.asteroid);
    }

    // SKYBOX DRAW START ------------------------
    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    Shader_Bind(ins.sky_box_shader);

    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection  = GLM_MAT4_IDENTITY_INIT;
    // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
    // The last row and column affect the translation of the skybox (which we don't want to affect)

    Camera_Get_View_Matrix(ins.cam, view_matrix);
    view_matrix[0][3]  = 0.0f;
    view_matrix[0][7]  = 0.0f;
    view_matrix[0][11] = 0.0f;
    view_matrix[0][15] = 0.0f;
    view_matrix[3][0]  = 0.0f;
    view_matrix[3][1]  = 0.0f;
    view_matrix[3][2]  = 0.0f;
    view_matrix[3][3]  = 0.0f;

    glm_perspective(glm_rad(45.0f), (float)window.width / window.heigh, 0.1f, 100.0f, projection);

    Shader_Uniform_Mat4(ins.sky_box_shader, "view", view_matrix);
    Shader_Uniform_Mat4(ins.sky_box_shader, "projection", projection);

    // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
    // where an object is present (a depth of 1.0f will always fail against any object's depth value)
    glBindVertexArray(ins.skybox_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ins.cubemap_texture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);

    // SKYBOX DRAW END --------------------------
}

void Instancing_OnExit()
{
    Camera_Print_Values(ins.cam);

    Mesh_Free(ins.asteroid);
    Mesh_Free(ins.jupiter);

    glDeleteVertexArrays(1, &ins.skybox_VAO);
    glDeleteTextures(1, &ins.cubemap_texture);
}
