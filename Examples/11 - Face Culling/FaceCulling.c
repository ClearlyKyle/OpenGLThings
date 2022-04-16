#include "FaceCulling.h"

static struct FaceCulling fc;

void FaceCulling_Init()
{
    // Generates Shader object using shaders default.vert and default.frag
    struct Shader shaderProgram = Shader_Create(
        "../../Examples/11 - Face Culling/default.vs",
        "../../Examples/11 - Face Culling/default.fs",
        4,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aNormal"},
            {.index = 2, .name = "aColour"},
            {.index = 3, .name = "aTex"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos = {1.0f, 1.0f, 1.5f};
    // mat4 lightModel;
    // glm_translate_make(lightModel, lightPos);

    Shader_Bind(shaderProgram);
    Shader_Uniform_Vec4(shaderProgram, "lightColor", lightColor);
    Shader_Uniform_Vec3(shaderProgram, "lightPos", lightPos);

    // Enables the Depth Buffer
    // glEnable(GL_DEPTH_TEST);

    // CULLING
    glEnable(GL_CULL_FACE); // Enables Cull Facing
                            // glCullFace(GL_FRONT);
    glCullFace(GL_BACK);    // specifies which face to cull
    glFrontFace(GL_CCW);    // Uses counter clock-wise standard

    // struct Mesh model = Mesh_Load("S:/Downloads/170516_mia337_032122_600_200Kfaces_8192px_zip/170516_mia337_032122_600_200Kfaces_8192px_OBJ.obj");
    struct Mesh model = Mesh_Load(shaderProgram, "../../Examples/res/models/statue/scene.gltf");
    // struct Mesh model = Mesh_Load(shaderProgram, "../../Examples/res/models/Wooden Box/wooden crate.obj");
    fc.model = model;

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    glm_vec3_copy((vec3){0.114410f, 0.412679f, 0.933686f}, cam.position);
    glm_vec3_copy((vec3){-0.188730f, -0.345298f, -0.919321f}, cam.orientation);
    cam.pitch = -20.199989f;
    cam.yaw = -101.601227f;

    fc.cam = cam;

    // Texture_Uniform(fc.model.shader, fc.model.textures[0], "diffuse0", fc.model.textures[0].slot);

    Shader_Uniform_Texture2D(fc.model.shader, "diffuse0", fc.model.textures[0]);
}

void FaceCulling_Update()
{
    // fprintf(stderr, "%fms : %f FPS\n", window.frame_time, 1.0f / window.frame_time);
    Camera_Inputs(&fc.cam);

    Shader_Uniform_Vec3(fc.model.shader, "camPos", fc.cam.position);
    Camera_View_Projection_To_Shader(fc.cam, fc.model.shader, "camMatrix");

    Mesh_Draw(fc.model);
}

void FaceCulling_OnExit()
{
    Camera_Print_Values(fc.cam);

    Mesh_Free(fc.model);
}
