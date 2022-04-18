#include "Transparancy.h"

static struct Transparancy {
    struct Camera cam;
    struct Mesh ground;
    struct Mesh grass;
    struct Mesh window;
} trans;

void Transparancy_Init()
{
    struct Shader shader_default = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                                 "../../Examples/12 - Transparancy/grass.vs",
                                                 0,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"}});

    struct Shader shader_grass = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                               "../../Examples/12 - Transparancy/grass.vs",
                                               0,
                                               (struct VertexAttribute[]){
                                                   {.index = 0, .name = "aPos"}});

    struct Shader shader_window = Shader_Create("../../Examples/12 - Transparancy/default.vs",
                                                "../../Examples/12 - Transparancy/grass.vs",
                                                0,
                                                (struct VertexAttribute[]){
                                                    {.index = 0, .name = "aPos"}});

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
    glEnable(GL_CULL_FACE);                            // Enables Cull Facing
    glCullFace(GL_FRONT);                              // Keeps front faces
    glFrontFace(GL_CCW);                               // Uses counter clock-wise standard
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Configures the blending function

    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    trans.cam         = cam;

    struct Mesh ground = Mesh_Load(shader_default, "../../Examples/res/ground/scene.gltf");
    struct Mesh grass  = Mesh_Load(shader_grass, "../../Examples/res/ground/scene.gltf");
    struct Mesh window = Mesh_Load(shader_window, "../../Examples/res/ground/scene.gltf");

    trans.ground = ground;
    trans.grass  = grass;
    trans.window = window;
}

void Transparancy_Update() {}

void Transparancy_OnExit()
{
    Mesh_Free(trans.ground);
    Mesh_Free(trans.grass);
    Mesh_Free(trans.window);
}
