#include "DepthBuffer.h"

static struct DepthBuffer dbuffer;

void DepthBuffer_Init()
{
    struct Shader shader = Shader_Create(
        "../../Examples/9 - Depth Buffer/depthbuffer.vs",
        "../../Examples/9 - Depth Buffer/depthbuffer.fs",
        3,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aNormal"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aColour"}});

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);

    vec4 light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 light_position = {1.5f, 5.5f, 1.5f};
    mat4 light_model;
    glm_translate_make(light_model, light_position);

    Shader_Bind(shader);
    Shader_Uniform_Vec4(shader, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader, "lightPos", light_position);

    // struct Mesh mesh = Mesh_load(shader, "../../Examples/res/models/Wooden Box/wooden crate.obj");
    struct Mesh mesh = Mesh_Load(shader, "../../Examples/res/models/Dog House/DogHouse.obj");
    // struct Mesh mesh = Mesh_Load(shader, "../../Examples/res/models/Low Poly Scene/Low-Poly_Models.obj");
    dbuffer.model = mesh;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    // glm_vec3_copy((vec3){-2.677339f, 1.266167f, -10.688577f}, cam.position);
    // glm_vec3_copy((vec3){0.683066f, -0.107999f, -0.722327f}, cam.orientation);
    // cam.pitch = -6.199975f;
    // cam.yaw   = -46.600174f;

    // DOG HOUSE CAMERA START POSITION
    glm_vec3_copy((vec3){-1.021186f, 0.806007f, 1.551936f}, cam.position);
    glm_vec3_copy((vec3){0.565596f, -0.263875f, -0.781327f}, cam.orientation);
    cam.pitch = -15.300121f;
    cam.yaw   = -54.099522f;

    dbuffer.cam = cam;
}

void DepthBuffer_Update()
{
    Camera_Inputs(&dbuffer.cam);

    Shader_Bind(dbuffer.model.shader);
    Shader_Uniform_Vec3(dbuffer.model.shader, "camPos", dbuffer.cam.position);
    Shader_Uniform_Vec3(dbuffer.model.shader, "camFront", dbuffer.cam.orientation);

    Camera_View_Projection_To_Shader(dbuffer.cam, dbuffer.model.shader, "camMatrix");

    Mesh_Draw(dbuffer.model);
}

void DepthBuffer_OnExit()
{
    Camera_Print_Values(dbuffer.cam);

    Mesh_Free(dbuffer.model);
}
