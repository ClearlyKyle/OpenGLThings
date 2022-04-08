#include "DepthBuffer.h"

static struct DepthBuffer dbuffer;

void DepthBuffer_Init()
{
    struct Shader shader = Shader_Create(
        "../../Examples/shaders/9/depthbuffer.vs",
        "../../Examples/shaders/9/depthbuffer.fs",
        3,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aNormal"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aColour"}});

    // vec4 light_colour = {1.0f, 1.0f, 1.0f, 1.0f};
    // vec3 light_position = {0.5f, 0.5f, 0.5f};
    // mat4 light_model;
    // glm_translate_make(light_model, light_position);

    // Shader_Bind(shader);
    // Shader_Uniform_Vec4(shader, "lightColor", light_colour);
    // Shader_Uniform_Vec3(shader, "lightPos", light_position);

    // struct Mesh mesh = Load_Model_Data("../../Examples/res/models/Wooden Box/wooden crate.obj");
    struct Mesh mesh = Load_Model_Data("../../Examples/res/models/Dog House/DogHouse.obj");
    // struct Mesh mesh = Load_Model_Data("../../Examples/res/models/Low Poly Scene/Low-Poly_Models.obj");
    mesh.shader = shader;
    dbuffer.model = mesh;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    // Camera_Set_Orientation(&cam, (vec3){-0.06f, -0.35f, 0.93f});
    // Camera_Set_Position(&cam, (vec3){
    //                               5.32f,
    //                               7.78f,
    //                               -37.99f,
    //                           });
    dbuffer.cam = cam;
}

void DepthBuffer_Update()
{
    Camera_Inputs(&dbuffer.cam);

    Model_Render_Mesh(dbuffer.model, dbuffer.cam);
}

void DepthBuffer_OnExit()
{
    glm_vec3_print(dbuffer.cam.position, stdout);
    glm_vec3_print(dbuffer.cam.orientation, stdout);
    glm_vec3_print(dbuffer.cam.up, stdout);

    Mesh_Free(dbuffer.model);
}
