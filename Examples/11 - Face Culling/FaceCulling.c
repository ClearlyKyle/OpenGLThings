#include "FaceCulling.h"

static struct FaceCulling fc;

void FaceCulling_Init() {
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shaderProgram = Shader_Create(
        "../../Examples/11 - Face Culling/default.vs",
        "../../Examples/11 - Face Culling/default.fs",
        4,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aNormal"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aColour"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos = {0.5f, 0.5f, 0.5f};
    mat4 lightModel;
    glm_translate_make(lightModel, lightPos);

    Shader_Bind(shaderProgram);
    Shader_Uniform_Vec4(shaderProgram, "lightColor", lightColor);
    Shader_Uniform_Vec3(shaderProgram, "lightPos", lightPos);

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // CULLING
    // Enables Cull Facing
    glEnable(GL_CULL_FACE);
    // Keeps front faces
    glCullFace(GL_FRONT);
    // Uses counter clock-wise standard
    glFrontFace(GL_CCW);

    struct Mesh model = Load_Model_Data("../../Examples/res/models/statue/scene.gltf");
    model.shader = shaderProgram;
    fc.model = model;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    // glm_vec3_copy((vec3){2.416749f, 10.969033f, 35.725922f}, cam.position);
    // glm_vec3_copy((vec3){-0.033144f, 0.005236f, -0.999437f}, cam.orientation);
    //  glm_vec3_copy((vec3){1.77349f, 13.74657f, 40.44918f}, cam.up);
    // cam.pitch = 0.800025f;
    // cam.yaw = -94.199196f;

    fc.cam = cam;
}

void FaceCulling_Update() {
    fprintf(stderr, "%f\n", window.frame_time);

    Shader_Bind(fc.model.shader);
    Shader_Uniform_Vec3(fc.model.shader, "camPos", fc.cam.position);

    Camera_View_Projection_To_Shader(fc.cam, fc.model.shader, "camMatrix");

    recursive_render(fc.model, fc.model.scene, fc.model.scene->mRootNode);
}

void FaceCulling_OnExit() {
    Camera_Print_Values(fc.cam);

    Mesh_Free(fc.model);
}
