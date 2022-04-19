#include "Transparancy.h"

static struct Transparancy
{
    struct Camera cam;
    struct Mesh ground;
    struct Mesh grass;
    struct Mesh window;

    struct Texture tex_tmp1;
    struct Texture tex_tmp2;
} trans;

#define NUMBER_OF_WINDOWS 100

// Generates all windows
vec3 window_positions[NUMBER_OF_WINDOWS];
float window_rotations[NUMBER_OF_WINDOWS];
unsigned int window_draw_order[NUMBER_OF_WINDOWS];
float distanceCamera[NUMBER_OF_WINDOWS];

int window_compare_function(const void *a, const void *b)
{
    const double diff = distanceCamera[*(int *)b] - distanceCamera[*(int *)a];
    return (0 < diff) - (diff < 0);
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
    // glEnable(GL_CULL_FACE);                            // Enables Cull Facing
    // glCullFace(GL_BACK);                               // Keeps front faces
    // glFrontFace(GL_CCW);                               // Uses counter clock-wise standard
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Configures the blending function

    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){-39.353584f, 8.195490f, 30.373175f}, cam.position);
    glm_vec3_copy((vec3){0.810961f, -0.213032f, -0.544940f}, cam.orientation);
    cam.pitch = -12.300069f;
    cam.yaw   = -33.899826f;

    trans.cam = cam;

    struct Mesh ground = Mesh_Load(shader_default, "../../Examples/res/models/ground/scene.gltf");
    // struct Mesh grass  = Mesh_Load(shader_grass, "../../Examples/res/models/grass/scene.gltf");
    // struct Mesh window = Mesh_Load(shader_window, "../../Examples/res/models/windows/scene.gltf");

    trans.ground = ground;
    // trans.grass  = grass;
    // trans.window = window;

    for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    {
        window_positions[i][0] = (float)(-15.0f + (rand()) / ((float)(RAND_MAX / (15.0f - (-15.0f)))));
        window_positions[i][1] = (float)(1.0f + (rand()) / ((float)(RAND_MAX / (4.0f - 1.0f))));
        window_positions[i][2] = (float)(-15.0f + (rand()) / ((float)(RAND_MAX / (15.0f - (-15.0f)))));

        window_rotations[i]  = (float)(rand()) / ((float)(RAND_MAX / 1.0f));
        window_draw_order[i] = (unsigned int)i;
    }

    const char *file_path1 = "../../Examples/res/models/ground/diffuse.png";
    struct Texture tex1    = Texture_Create(file_path1, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    trans.tex_tmp1         = tex1;
    const char *file_path2 = "../../Examples/res/models/ground/specular.png";
    struct Texture tex2    = Texture_Create(file_path2, GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    trans.tex_tmp2         = tex2;

    Shader_Uniform_Texture2D(shader_default, "diffuse0", tex1);
    Shader_Uniform_Texture2D(shader_default, "specular0", tex2);
}

void Transparancy_Update()
{
    Camera_Inputs(&trans.cam);

    Shader_Uniform_Vec3(trans.ground.shader, "camPos", trans.cam.position);
    Camera_View_Projection_To_Shader(trans.cam, trans.ground.shader, "camMatrix");

    // Shader_Uniform_Vec3(trans.grass.shader, "camPos", trans.cam.position);
    // Camera_View_Projection_To_Shader(trans.cam, trans.grass.shader, "camMatrix");

    // Shader_Uniform_Vec3(trans.window.shader, "camPos", trans.cam.position);
    // Camera_View_Projection_To_Shader(trans.cam, trans.window.shader, "camMatrix");

    Texture_Bind(trans.tex_tmp1);
    Texture_Bind(trans.tex_tmp2);
    Mesh_Draw(trans.ground);

    //// Disable cull face so that grass and windows have both faces
    // glDisable(GL_CULL_FACE);
    // Mesh_Draw(trans.grass);

    // glEnable(GL_BLEND); // Enable blending for windows

    //// Get distance from each window to the camera
    // for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    //{
    //     distanceCamera[i] = glm_vec3_distance(trans.cam.position, window_positions[i]);
    //     // distanceCamera[i] = glm_vec3_distance2(trans.cam.position, window_positions[i]);
    // }

    //// Sort windows by distance from camera
    // qsort(window_draw_order, NUMBER_OF_WINDOWS, sizeof(unsigned int), window_compare_function);

    //// Draw windows
    // for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
    //{
    //     Mesh_Set_Translation(&trans.window, window_positions[window_draw_order[i]]);
    //     Mesh_Set_Rotation(&trans.window, (vec4){1.0f, 0.0f, window_rotations[window_draw_order[i]], 0.0f});
    //     Mesh_Draw(trans.window);
    // }

    // glDisable(GL_BLEND);
    // glEnable(GL_CULL_FACE);
}

void Transparancy_OnExit()
{
    Camera_Print_Values(trans.cam);

    Mesh_Free(trans.ground);
    Mesh_Free(trans.grass);
    Mesh_Free(trans.window);
}
