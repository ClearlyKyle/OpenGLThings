#include "Instancing.h"

#include <time.h>
#include "stb/stb_image.h"

#define NUMBER_OF_ASTEROIDS 500

static struct Instancing
{
    struct Mesh   jupiter;
    struct Mesh   asteroid_normal;
    struct Mesh   asteroid_instanced;
    struct Camera cam;

    bool switch_between_instance_rendering;

    Shader_t default_shader;
    Shader_t asteroid_shader;

    Skybox_t skybox;

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

static void Normal_Instanced_Method()
{
    struct Mesh asteroid = Mesh_Load(ins.default_shader, "../../Examples/res/models/asteroid/scene.gltf");
    ins.asteroid_normal  = asteroid;

    /* We manually setup the positons, scale, rotaion for all instanced then draw
     *  them all one by one
     */
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

static void Shader_Instanced_Method()
{
    const unsigned int number_of_instances = NUMBER_OF_ASTEROIDS;

    const float radius           = 100.0f; // Radius of circle around which asteroids orbit
    const float radius_deviation = 25.0f;  // How much ateroids deviate from the radius

    vec3 translation;
    vec4 rotation;
    vec3 scale;

    mat4 *matricies = malloc(sizeof(*matricies) * number_of_instances);
    check_that(matricies != NULL, "Error allocating matrecies\n");

    for (unsigned int i = 0; i < number_of_instances; i++)
    {
        // srand((unsigned int)time(NULL));
        //  Initialize matrices
        mat4 mat_trans = GLM_MAT4_IDENTITY_INIT;
        mat4 mat_rot   = GLM_MAT4_IDENTITY_INIT;
        mat4 mat_scale = GLM_MAT4_IDENTITY_INIT;

        const float x           = randf();
        const float finalRadius = radius + randf() * radius_deviation;
        const float y           = ((rand() % 2) * 2 - 1) * (float)sqrt(1.0f - x * x);

        if (randf() > 0.5f)
            glm_vec3_copy((vec3){y * finalRadius, randf(), x * finalRadius}, translation);
        else
            glm_vec3_copy((vec3){x * finalRadius, randf(), y * finalRadius}, translation);

        glm_vec4_copy((vec4){1.0f, randf(), randf(), randf()}, rotation);

        float scale_value = (0.2f * randf());
        if (scale_value < 0.0f)
            scale_value *= -1.0f;
        glm_vec3_copy((vec3){scale_value, scale_value, scale_value}, scale);

        // Transform the matrices to their correct form
        glm_translate_make(mat_trans, translation);
        glm_quat_mat4(rotation, mat_rot);
        glm_scale_make(mat_scale, scale);

        // trans * rot * sca
        glm_mat4_mul(mat_trans, mat_rot, matricies[i]);
        glm_mat4_mul(matricies[i], mat_scale, matricies[i]);
    }

    struct Mesh asteroid   = Mesh_Load_Instancing(ins.asteroid_shader, number_of_instances, matricies, "../../Examples/res/models/asteroid/scene.gltf");
    ins.asteroid_instanced = asteroid;

    free(matricies);
}

void Instancing_Init()
{
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

    struct Shader asteroid_shader = Shader_Create("../../Examples/16 - Instancing/asteroid.vs",
                                                  "../../Examples/16 - Instancing/default.fs",
                                                  5,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aNormal"},
                                                      {.index = 2, .name = "aTex"},
                                                      {.index = 3, .name = "aColor"},
                                                      {.index = 4, .name = "instanceMatrix"}});

    ins.default_shader  = shader_default;
    ins.asteroid_shader = asteroid_shader;

    // All the faces of the cubemap (make sure they are in this exact order)
    const char *cube_map_face_images[6] =
        {
            "../../Examples/res/textures/skybox/right.jpg",
            "../../Examples/res/textures/skybox/left.jpg",
            "../../Examples/res/textures/skybox/top.jpg",
            "../../Examples/res/textures/skybox/bottom.jpg",
            "../../Examples/res/textures/skybox/front.jpg",
            "../../Examples/res/textures/skybox/back.jpg"};

    Skybox_t skybox = Skybox_Create(skybox_shader, cube_map_face_images);
    ins.skybox      = skybox;

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(asteroid_shader);
    Shader_Uniform_Vec4(asteroid_shader, "lightColor", lightColor);
    Shader_Uniform_Vec3(asteroid_shader, "lightPos", lightPos);

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);    // Keeps front faces
    glFrontFace(GL_CCW);     // Uses counter clock-wise standard

    // Load in models
    struct Mesh jupiter = Mesh_Load(shader_default, "../../Examples/res/models/jupiter/scene.gltf");
    ins.jupiter         = jupiter;

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-155.146210f, 60.443268f, -131.134857f}, cam.position);
    glm_vec3_copy((vec3){0.843958f, -0.381072f, 0.377516f}, cam.orientation);
    cam.pitch = -22.400122f;
    cam.yaw   = 24.099716f;
    ins.cam   = cam;

    Normal_Instanced_Method();
    Shader_Instanced_Method();

    ins.switch_between_instance_rendering = true;
}

static void Normal_Draw_Method()
{
    // Normal non instanced method, updating astroids one by one and drawing them seperatly
    // So many draw calls is slow and expensive $$
    for (unsigned int i = 0; i < NUMBER_OF_ASTEROIDS; i++)
    {
        Mesh_Set_Translation(&ins.asteroid_normal, ins.translations[i]);
        Mesh_Set_Rotation(&ins.asteroid_normal, ins.rotations[i]);
        Mesh_Set_Scale(&ins.asteroid_normal, ins.scales[i]);
        Mesh_Draw(ins.asteroid_normal);
    }
}

static void Instanced_Draw_Method()
{
    Mesh_Draw(ins.asteroid_instanced);
}

void Instancing_Update()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);            // Specify the color of the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the back buffer and depth buffer

    Camera_Inputs(&ins.cam);

    Shader_Uniform_Vec3(ins.jupiter.shader, "camPos", ins.cam.position);
    Camera_View_Projection_To_Shader(ins.cam, ins.jupiter.shader, "camMatrix");

    Mesh_Draw(ins.jupiter);

    if (Input_Key_Pressed(window.input, SDL_SCANCODE_F))
    {
        printf("Switching render modes\n");
        ins.switch_between_instance_rendering = !ins.switch_between_instance_rendering;
    }

    if (ins.switch_between_instance_rendering)
    {
        // Draw the asteroids
        Shader_Uniform_Vec3(ins.asteroid_instanced.shader, "camPos", ins.cam.position);
        Camera_View_Projection_To_Shader(ins.cam, ins.asteroid_instanced.shader, "camMatrix");

        Instanced_Draw_Method();
    }
    else
    {
        // Draw the normal method
        Shader_Uniform_Vec3(ins.asteroid_normal.shader, "camPos", ins.cam.position);
        Camera_View_Projection_To_Shader(ins.cam, ins.asteroid_normal.shader, "camMatrix");

        Normal_Draw_Method();
    }

    Skybox_Draw(ins.skybox, ins.cam);
}

void Instancing_OnExit()
{
    Camera_Print_Values(ins.cam);

    Mesh_Free(ins.asteroid_instanced);
    Mesh_Free(ins.asteroid_normal);
    Mesh_Free(ins.jupiter);

    Shader_Destroy(&ins.default_shader);
    Shader_Destroy(&ins.asteroid_shader);

    Skybox_Free(ins.skybox);
}
