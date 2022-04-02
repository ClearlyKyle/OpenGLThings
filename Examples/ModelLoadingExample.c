#include "ModelLoadingExample.h"

static struct ModelLoading model_loading;

void ModelLoading_Init()
{
    struct Model model = Model_Import(
        //"../../res/models/Dog House/Doghouse.obj",
        //"../../res/models/Wooden Box/wooden crate.obj",
        "../../res/models/cube_normals.obj",
        "../../shaders/12/default.vs",
        "../../shaders/12/default.fs",
        1,
        (struct VertexAttribute[]){{.index = 0, .name = "aPos"}, {.index = 1, .name = "aNormal"}});

    model_loading.model = model;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    model_loading.camera = cam;
}

void ModelLoading_Update()
{
    // Handles camera inputs
    Camera_Inputs(&model_loading.camera);

    // Tells OpenGL which Shader Program we want to use
    Shader_Bind(model_loading.model.shader);

    // Exports the camera Position to the Fragment Shader for specular lighting
    Shader_Uniform_Vec3(model_loading.model.shader, "camPos", model_loading.camera.position);

    // Export the camMatrix to the Vertex Shader of the pyramid
    Camera_View_Projection_To_Shader(model_loading.camera, model_loading.model.shader, "camMatrix");

    // Bind the VAO so OpenGL knows to use it
    VAO_Bind(model_loading.model.vao);

    // Draw primitives, number of indices, datatype of indices, index of indices
    // glDrawElements(GL_TRIANGLES, model_loading.model.num_indicies, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, model_loading.model.num_indicies, GL_UNSIGNED_INT, 0);
}

void ModelLoading_OnExit()
{
    VAO_Destroy(model_loading.vao);
    Shader_Destroy(model_loading.shader);
}