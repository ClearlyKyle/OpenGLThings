#include "StencilBuffer.h"

static struct StencilBuffer sb;

void StencilBuffer_Init()
{
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shaderProgram = Shader_Create(
        "../../Examples/10 - Stencil Buffer/default.vs",
        "../../Examples/10 - Stencil Buffer/default.fs",
        4,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aNormal"},
            {.index = 2, .name = "aTex"},
            {.index = 3, .name = "aColour"}});

    // Shader for the outlining model
    struct Shader outliningProgram = Shader_Create(
        "../../Examples/10 - Stencil Buffer/outlining.vs",
        "../../Examples/10 - Stencil Buffer/outlining.fs",
        1,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos = {0.5f, 0.5f, 0.5f};
    mat4 lightModel;
    glm_translate_make(lightModel, lightPos);

    Shader_Bind(shaderProgram);
    Shader_Uniform_Vec4(shaderProgram, "lightColor", lightColor);
    Shader_Uniform_Vec3(shaderProgram, "lightPos", lightPos);

    struct Mesh model = Mesh_Load(shaderProgram, "../../Examples/res/models/crow/scene.gltf");
    // model.shader = shaderProgram;
    sb.model = model;

    struct Mesh outline = Mesh_Load(outliningProgram, "../../Examples/res/models/crow-outline/scene.gltf");
    // outline.shader = outliningProgram;
    sb.outline = outline;

    sb.model_shader = shaderProgram;
    sb.outline_shader = outliningProgram;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){2.416749f, 10.969033f, 35.725922f}, cam.position);
    glm_vec3_copy((vec3){-0.033144f, 0.005236f, -0.999437f}, cam.orientation);
    // glm_vec3_copy((vec3){1.77349f, 13.74657f, 40.44918f}, cam.up);
    cam.pitch = 0.800025f;
    cam.yaw = -94.199196f;

    sb.cam = cam;

    glEnable(GL_DEPTH_TEST);   // Enables the Depth Buffer
    glEnable(GL_STENCIL_TEST); // Enables the Stencil Buffer
    glStencilOp(
        GL_KEEP,   // stencil action if stencil test fails
        GL_KEEP,   // stencil action if stencil test passes but depth test fails
        GL_REPLACE // stencil action if both pass (fragment shader will also run)
    );
}

void StencilBuffer_Update()
{
    glClear(GL_STENCIL_BUFFER_BIT);

    Camera_Inputs(&sb.cam);

    // ------------------------------------------------------------------------------------------------
    // MODEL SHADER BIND
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Make it so the stencil test always passes
    glStencilMask(0xFF);               // Enable modifying of the stencil buffer

    Shader_Bind(sb.model.shader);
    Shader_Uniform_Vec3(sb.model.shader, "camPos", sb.cam.position);

    Camera_View_Projection_To_Shader(sb.cam, sb.model.shader, "camMatrix");

    // Draw the normal model
    Mesh_Draw(sb.model);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Make it so only the pixels without the value 1 pass the test
    glStencilMask(0x00);                 // Disable modifying of the stencil buffer
    glDisable(GL_DEPTH_TEST);            // Disable the depth buffer

    // ------------------------------------------------------------------------------------------------
    // OUTLINE SHADER BIND
    Shader_Bind(sb.outline_shader);
    Shader_Uniform_Vec3(sb.outline.shader, "camPos", sb.cam.position);

    Camera_View_Projection_To_Shader(sb.cam, sb.outline.shader, "camMatrix");

    // First method from the tutorial
    // Shader_Uniform_Float(sb.outline.shader, "outlining", 1.08f);
    // Shader_Bind(sb.model_shader);
    // sb.model.shader = sb.outline_shader;                                   // TODO : Models should not hold their own shader?
    // recursive_render(sb.model, sb.model.scene, sb.model.scene->mRootNode); // DRAW MODEL
    // sb.model.shader = sb.model_shader;

    // Second method from the tutorial
    // Shader_Uniform_Float(sb.outline.shader, "outlining", 0.08f);
    // recursive_render(sb.outline, sb.outline.scene, sb.outline.scene->mRootNode);

    // Third method from the tutorial
    // sb.outline.shader = sb.outline_shader;
    Mesh_Draw(sb.outline);

    // Reset and Clear
    glStencilMask(0xFF);               // Enable modifying of the stencil buffer
    glStencilFunc(GL_ALWAYS, 0, 0xFF); // Clear stencil buffer
    glEnable(GL_DEPTH_TEST);           // Enable the depth buffer
}

void StencilBuffer_OnExit()
{
    Camera_Print_Values(sb.cam);

    Mesh_Free(sb.model);
    Mesh_Free(sb.outline);
}
