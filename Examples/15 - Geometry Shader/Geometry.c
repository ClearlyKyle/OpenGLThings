#include "Geometry.h"

static struct Geometry
{
    struct Mesh   model;
    struct Camera cam;

    struct Shader default_shader;
    struct Shader normal;
} gs;

void Geometry_Init()
{
    // Generates Shader object using shaders default.vert and default.frag
    // Shader        shaderProgram("default.vert", "default.frag", "default.geom");
    struct Shader shader_default = Shader_Create2("../../Examples/15 - Geometry Shader/default.vs",
                                                  "../../Examples/15 - Geometry Shader/default.fs",
                                                  "../../Examples/15 - Geometry Shader/default.gs",
                                                  4,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aNormal"},
                                                      {.index = 2, .name = "aColor"},
                                                      {.index = 3, .name = "aTex"}});

    // Shader        normalsShader("default.vert", "normals.frag", "normals.geom");
    struct Shader normals_shader = Shader_Create2("../../Examples/15 - Geometry Shader/default.vs",
                                                  "../../Examples/15 - Geometry Shader/normals.fs",
                                                  "../../Examples/15 - Geometry Shader/normals.gs",
                                                  4,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aNormal"},
                                                      {.index = 2, .name = "aColor"},
                                                      {.index = 3, .name = "aTex"}});

    gs.default_shader = shader_default;
    gs.normal         = normals_shader;

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);    // Keeps front faces
    glFrontFace(GL_CCW);     // Uses counter clock-wise standard

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);

    // glm_vec3_copy((vec3){4.037022f, 0.606838f, -4.618166f}, cam.position);
    // glm_vec3_copy((vec3){-0.679725f, -0.080201f, 0.729069f}, cam.orientation);
    // cam.pitch = -4.600101f;
    // cam.yaw   = -227.006027f;

    gs.cam = cam;

    // Load in models
    struct Mesh model = Mesh_Load(shader_default, "../../Examples/res/models/statue/scene.gltf");
    gs.model          = model;
}

void Geometry_Update()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera_Inputs(&gs.cam);

    // Draw the normal model
    gs.model.shader = gs.default_shader;
    Shader_Uniform_Vec3(gs.model.shader, "camPos", gs.cam.position);
    Camera_View_Projection_To_Shader(gs.cam, gs.model.shader, "camMatrix");
    Mesh_Draw(gs.model);

    // Draw the model showing NORMALS
    gs.model.shader = gs.normal;
    Shader_Uniform_Vec3(gs.model.shader, "camPos", gs.cam.position);
    Camera_View_Projection_To_Shader(gs.cam, gs.model.shader, "camMatrix");
    Mesh_Draw(gs.model);
}

void Geometry_OnExit()
{
    Camera_Print_Values(gs.cam);

    Mesh_Free(gs.model);
}
