#include "Transparancy.h"

#include <time.h>

#define NUMBER_OF_WINDOWS 100

static struct Transparancy
{
    struct Camera cam;
    struct Mesh ground;
    struct Mesh grass;
    struct Mesh window;

    // Window Texture Properties
    vec3 window_positions[NUMBER_OF_WINDOWS];
    float window_rotations[NUMBER_OF_WINDOWS];
    float distanceCamera[NUMBER_OF_WINDOWS];
    unsigned int window_draw_order[NUMBER_OF_WINDOWS];
} trans;

int window_compare_function(const void *a, const void *b)
{
    const double diff = trans.distanceCamera[*(int *)b] - trans.distanceCamera[*(int *)a];
    return (0 < diff) - (diff < 0);
}

float FLOAT_RAND(float min, float max)
{
    const float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);             /* [min, max] */
}

void Transparancy_Init()
{
    struct Shader shader_default = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                                 "../../Examples/12 - Transparancy/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aColor"},
                                                     {.index = 3, .name = "aTex"}});

    struct Shader shader_grass  = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                                "../../Examples/12 - Transparancy/grass.fs",
                                                4,
                                                (struct VertexAttribute[]){
                                                    {.index = 0, .name = "aPos"},
                                                    {.index = 1, .name = "aNormal"},
                                                    {.index = 2, .name = "aColor"},
                                                    {.index = 3, .name = "aTex"}});
    struct Shader shader_window = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                                "../../Examples/12 - Transparancy/windows.fs",
                                                4,
                                                (struct VertexAttribute[]){
                                                    {.index = 0, .name = "aPos"},
                                                    {.index = 1, .name = "aNormal"},
                                                    {.index = 2, .name = "aColor"},
                                                    {.index = 3, .name = "aTex"}});
    // Take care of all the light related things
    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader_default, "lightPos", light_position);

    Shader_Bind(shader_grass);
    Shader_Uniform_Vec4(shader_grass, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader_grass, "lightPos", light_position);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);                            // Enables Cull Facing
    glCullFace(GL_BACK);                               // Keeps front faces
    glFrontFace(GL_CCW);                               // Uses counter clock-wise standard
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Configures the blending function

    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){-39.353584f, 8.195490f, 30.373175f}, cam.position);
    glm_vec3_copy((vec3){0.810961f, -0.213032f, -0.544940f}, cam.orientation);
    cam.pitch = -12.300069f;
    cam.yaw   = -33.899826f;

    trans.cam = cam;

    // struct Mesh ground = Mesh_Load(shader_default, "../../Examples/res/models/ground/ground.obj"); // Y axis flipped
    struct Mesh ground = Mesh_Load(shader_default, "../../Examples/res/models/ground/scene.gltf");
    struct Mesh grass  = Mesh_Load(shader_grass, "../../Examples/res/models/grass/scene.gltf");
    struct Mesh window = Mesh_Load(shader_window, "../../Examples/res/models/windows/scene.gltf");

    trans.ground = ground;
    trans.grass  = grass;
    trans.window = window;

    srand((unsigned int)time(NULL));

    for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    {
        trans.window_positions[i][0] = (float)(FLOAT_RAND(-15.0f, 15.0f));
        trans.window_positions[i][1] = (float)(FLOAT_RAND(2.0f, 8.0f));
        trans.window_positions[i][2] = (float)(FLOAT_RAND(-15.0f, 15.0f));

        trans.window_rotations[i]  = (float)(FLOAT_RAND(-1.0f, 1.0f));
        trans.window_draw_order[i] = (unsigned int)i;
    }
}

void Transparancy_Update()
{
    Camera_Inputs(&trans.cam);

    Shader_Uniform_Vec3(trans.ground.shader, "camPos", trans.cam.position);
    Camera_View_Projection_To_Shader(trans.cam, trans.ground.shader, "camMatrix");

    Shader_Uniform_Vec3(trans.grass.shader, "camPos", trans.cam.position);
    Camera_View_Projection_To_Shader(trans.cam, trans.grass.shader, "camMatrix");

    Shader_Uniform_Vec3(trans.window.shader, "camPos", trans.cam.position);
    Camera_View_Projection_To_Shader(trans.cam, trans.window.shader, "camMatrix");

    Mesh_Draw(trans.ground);

    // Disable cull face so that grass and windows have both faces
    Mesh_Draw(trans.grass);

    glEnable(GL_BLEND); // Enable blending for windows

    // Get distance from each window to the camera
    for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    {
        trans.distanceCamera[i] = glm_vec3_distance(trans.cam.position, trans.window_positions[i]);
        // distanceCamera[i] = glm_vec3_distance2(trans.cam.position, trans.window_positions[i]);
    }

    // Sort windows by distance from camera
    qsort(trans.window_draw_order, NUMBER_OF_WINDOWS, sizeof(*trans.window_draw_order), window_compare_function);

    // Draw windows
    for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    {
        Mesh_Set_Translation(&trans.window, trans.window_positions[trans.window_draw_order[i]]);
        Mesh_Set_Rotation(&trans.window, (vec4){1.0f, 0.0f, trans.window_rotations[trans.window_draw_order[i]], 0.0f});
        Mesh_Draw(trans.window);
    }

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

void Transparancy_OnExit()
{
    Camera_Print_Values(trans.cam);

    Mesh_Free(trans.ground);
    Mesh_Free(trans.grass);
    Mesh_Free(trans.window);
}
