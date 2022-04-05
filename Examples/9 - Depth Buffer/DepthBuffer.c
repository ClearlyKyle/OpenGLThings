#include "DepthBuffer.h"

static struct DepthBuffer dbuffer;

void DepthBuffer_Init()
{
    struct Shader shader = Shader_Create(
        "../../Examples/shaders/9/depthbuffer.vs",
        "../../Examples/shaders/9/depthbuffer.fs",
        1,
        (struct VertexAttribute[]){
            {.name = "aPos", .index = 0}});

    //vec4 light_colour = {1.0f, 1.0f, 1.0f, 1.0f};
    //vec3 light_position = {0.5f, 0.5f, 0.5f};
    //mat4 light_model;
    //glm_translate_make(light_model, light_position);

    //Shader_Bind(shader);
    //Shader_Uniform_Vec4(shader, "lightColor", light_colour);
    //Shader_Uniform_Vec3(shader, "lightPos", light_position);
    dbuffer.shader = shader;

    //struct Model ground = Model_ASSIMP("../../Examples/res/models/Cube/cube_triang.obj", shader);
    struct Model ground = Model_ASSIMP("../../Examples/res/models/Dog House/DogHouse.obj", shader);
    //    struct Model trees = Model_Import_Shader();
    dbuffer.model = ground;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    dbuffer.cam = cam;
}

void DepthBuffer_Update()
{
    Camera_Inputs(&dbuffer.cam);
    // window.quit = true;
    Model_Draw(dbuffer.model, dbuffer.cam);
}

void DepthBuffer_OnExit()
{
    VAO_Destroy(dbuffer.vao);
    Shader_Destroy(dbuffer.shader);
}
