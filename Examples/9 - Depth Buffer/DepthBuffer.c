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
    dbuffer.model.shader = shader;
    dbuffer.shader = shader;

    // struct Model ground = Model_ASSIMP("../../Examples/res/models/Low Poly Scene/Low-Poly_Models.obj", shader);
    // struct Model ground = Model_ASSIMP("../../Examples/res/models/Cube/cube_triang.obj", shader);
    // struct Model ground = Model_ASSIMP("../../Examples/res/models/Dog House/DogHouse.obj", shader);
    //     struct Model trees = Model_Import_Shader();

    // struct Mesh mesh = Load_Model_Data("../../Examples/res/models/Dog House/DogHouse.obj");
    struct Mesh mesh = Load_Model_Data("../../Examples/res/models/Low Poly Scene/Low-Poly_Models.obj");
    dbuffer.model = mesh;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    dbuffer.cam = cam;
}

void DepthBuffer_Update()
{
    Camera_Inputs(&dbuffer.cam);

    Shader_Bind(dbuffer.shader);

    // struct Shader s = dbuffer.shader;

    // Camera Matrixd
    Shader_Uniform_Vec3(dbuffer.shader, "camPos", dbuffer.cam.position);
    Camera_View_Projection_To_Shader(dbuffer.cam, dbuffer.shader, "camMatrix");

    vec3 model_position = {0.0f, 0.0f, -2.0f};
    mat4 model_transform;
    glm_translate_make(model_transform, model_position);

    Shader_Uniform_Mat4(dbuffer.shader, "model", model_transform);

    // window.quit = true;
    Model_Render_Mesh(dbuffer.model, dbuffer.cam, dbuffer.model.shader);
}

void DepthBuffer_OnExit()
{
    VAO_Destroy(dbuffer.vao);
    Shader_Destroy(dbuffer.shader);
}
