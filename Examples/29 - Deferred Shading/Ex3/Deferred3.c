#include "Deferred3.h"

#include "GBuffer.h"

// Example from...
// https://ogldev.org/www/tutorial36/tutorial36.html

#define NUMBER_OF_BOXES 5
#define NUM_OF_POINT_LIGHTS 3

static struct DS
{
    Camera_t cam;
    Mesh_t   sphere_mesh;

    float scale;

    GBuffer_t gBuf;
    Shader_t  shader_geometry_pass;
    Shader_t  shader_directional_light_pass;
    Shader_t  shader_point_light_pass;

    Texture_t tex_cube;

    VAO_t cube;
    VAO_t quad;

    vec3 cube_positions[NUMBER_OF_BOXES];

    Spotlight_t        spotlight;
    DirectionalLight_t dir_light;
    PointLight_t       point_light[NUM_OF_POINT_LIGHTS];

} ds;

static void DSGeometryPass()
{
    Shader_Bind(ds.shader_geometry_pass);

    GBuffer_BindForWriting(&ds.gBuf);

    // Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    Camera_View_Projection_To_Shader(ds.cam, ds.shader_geometry_pass, "camMatrix");

    mat4 model; // aka world
    VAO_Bind(ds.cube);
    Texture_Bind(ds.tex_cube);
    for (unsigned int i = 0; i < NUMBER_OF_BOXES; i++)
    {
        glm_translate_make(model, ds.cube_positions[i]);
        // glm_scale(model, (vec3){0.1f, 0.1f, 0.1f});

        Shader_Uniform_Mat4(ds.shader_geometry_pass, "model", model);

        // Render cube
        Cube_Draw(ds.cube);
    }

    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
}

static void DSLightPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GBuffer_BindForReading(&ds.gBuf);

    const GLint WINDOW_WIDTH  = window.width;
    const GLint WINDOW_HEIGHT = window.heigh;

    const GLsizei HalfWidth  = (GLsizei)(WINDOW_WIDTH / 2.0f);
    const GLsizei HalfHeight = (GLsizei)(WINDOW_HEIGHT / 2.0f);

    GBuffer_SetReadBuffer(GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                      0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    GBuffer_SetReadBuffer(GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                      0, HalfHeight, HalfWidth, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    GBuffer_SetReadBuffer(GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                      HalfWidth, HalfHeight, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // GBuffer_SetReadBuffer(GBUFFER_TEXTURE_TYPE_TEXCOORD);
    // glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    //                   HalfWidth, 0, WINDOW_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

static void BeginLightPasses()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    GBuffer_BindForReading(&ds.gBuf);

    glClear(GL_COLOR_BUFFER_BIT);
}

// The calculation solves a quadratic equation (see http://en.wikipedia.org/wiki/Quadratic_equation)
// https://ogldev.org/www/tutorial36/threshold.jpg
static float Calculate_Point_Light_Sphere(const PointLight_t *light)
{
    const float max_colour_channel = max(max(light->colour[0], light->colour[1]), light->colour[2]);

    const float ret = (-light->attenuation.linear + sqrtf(light->attenuation.linear * light->attenuation.linear - 4 * light->attenuation.exp * (light->attenuation.exp - 256.0f * max_colour_channel * light->diffuse_intensity)) /
                                                        (2 * light->attenuation.exp));
    return ret;
}

// POINT LIGHT Pass
static void DSPointLightsPass()
{
    Shader_Bind(ds.shader_point_light_pass);

    Uniform_Vec3("gCameraPosition", ds.cam.position);

    Camera_View_Projection_To_Shader(ds.cam, ds.shader_point_light_pass, "camMatrix");

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    for (unsigned int i = 0; i < NUM_OF_POINT_LIGHTS; i++)
    {
        PointLight_To_Shader(ds.point_light[i], ds.shader_point_light_pass);

        const float BSphereScale = Calculate_Point_Light_Sphere(&ds.point_light[i]);

        glm_translate_make(model, ds.point_light[i].position);
        glm_scale(model, (vec3){BSphereScale, BSphereScale, BSphereScale});

        Uniform_Mat4("model", model);

        ds.sphere_mesh.shader = ds.shader_point_light_pass;
        Mesh_Draw(ds.sphere_mesh);
    }
}

// DIRECITONAL LIGHT Pass
static void DSDirectionalLightPass()
{
    mat4 model = GLM_MAT4_IDENTITY_INIT;

    Shader_Bind(ds.shader_directional_light_pass);

    Uniform_Vec3("gCameraPosition", ds.cam.position);
    Uniform_Mat4("model", model);
    Uniform_Mat4("camMatrix", model); // We dont need to transform the directional light?

    Quad_Draw(ds.quad);
}

void Deferred3_Init()
{
    // Camera, dont forget this!!
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-23.564487f, 5.983703f, 13.378875f}, cam.position);
    glm_vec3_copy((vec3){0.975138f, -0.221571f, -0.003419f}, cam.orientation);
    cam.pitch = -12.801350f;
    cam.yaw   = -0.200907f;

    // DONT TOUCH THIS PLSSS
    ds.cam = cam;

    // Initialise cube positions
    glm_vec3_copy((vec3){0.0f, 0.0f, 5.0f}, ds.cube_positions[0]);
    glm_vec3_copy((vec3){6.0f, 1.0f, 10.0f}, ds.cube_positions[1]);
    glm_vec3_copy((vec3){-5.0f, -1.0f, 12.0f}, ds.cube_positions[2]);
    glm_vec3_copy((vec3){4.0f, 4.0f, 15.0f}, ds.cube_positions[3]);
    glm_vec3_copy((vec3){-4.0f, 2.0f, 20.0f}, ds.cube_positions[4]);

    // Initialise Lights
    // SPOTLIGHT
    ds.spotlight.ambient_intensity  = 0.0f;
    ds.spotlight.diffuse_intensity  = 0.9f;
    ds.spotlight.attenuation.linear = 0.01f;
    ds.spotlight.cutoff             = 20.0f;
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, ds.spotlight.colour);
    glm_vec3_copy((vec3){-20.0f, 20.0f, 5.0f}, ds.spotlight.position);
    glm_vec3_copy((vec3){1.0f, -1.0f, 0.0f}, ds.spotlight.direction);

    // POINTLIGHT
    ds.point_light[0].diffuse_intensity    = 0.2f;
    ds.point_light[0].attenuation.constant = 0.0f;
    ds.point_light[0].attenuation.linear   = 0.0f;
    ds.point_light[0].attenuation.exp      = 0.3f;
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, ds.point_light[0].colour);
    glm_vec3_copy((vec3){0.0f, 1.5f, 5.0f}, ds.point_light[0].position);

    ds.point_light[1].diffuse_intensity    = 0.2f;
    ds.point_light[1].attenuation.constant = 0.0f;
    ds.point_light[1].attenuation.linear   = 0.0f;
    ds.point_light[1].attenuation.exp      = 0.3f;
    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, ds.point_light[1].colour);
    glm_vec3_copy((vec3){2.0f, 0.0f, 5.0f}, ds.point_light[1].position);

    ds.point_light[2].diffuse_intensity    = 0.2f;
    ds.point_light[2].attenuation.constant = 0.0f;
    ds.point_light[2].attenuation.linear   = 0.0f;
    ds.point_light[2].attenuation.exp      = 0.3f;
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, ds.point_light[2].colour);
    glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, ds.point_light[2].position);

    // DIRECTIONAL LIGHT
    ds.dir_light.ambient_intensity = 0.1f;
    ds.dir_light.diffuse_intensity = 0.5f;
    glm_vec3_copy((vec3){0.0f, 1.0f, 1.0f}, ds.dir_light.colour);
    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, ds.dir_light.direction);

    // Shader
    Shader_t shader_geometry_pass = Shader_Create("../../Examples/29 - Deferred Shading/Ex3/geo_pass.vs",
                                                  "../../Examples/29 - Deferred Shading/Ex3/geo_pass.fs",
                                                  3,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "Position"},
                                                      {.index = 1, .name = "Normal"},
                                                      {.index = 2, .name = "TexCoord"},
                                                  });

    Shader_t shader_directional_light_pass = Shader_Create("../../Examples/29 - Deferred Shading/Ex3/light_pass.vs",
                                                           "../../Examples/29 - Deferred Shading/Ex3/dir_light_pass.fs",
                                                           1,
                                                           (struct VertexAttribute[]){
                                                               {.index = 0, .name = "Position"},
                                                           });

    Shader_t shader_point_light_pass = Shader_Create("../../Examples/29 - Deferred Shading/Ex3/light_pass.vs",
                                                     "../../Examples/29 - Deferred Shading/Ex3/point_light_pass.fs",
                                                     1,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "Position"},
                                                     });

    ds.shader_geometry_pass          = shader_geometry_pass;
    ds.shader_directional_light_pass = shader_directional_light_pass;
    ds.shader_point_light_pass       = shader_point_light_pass;

    // Load in model
    Mesh_t sphere_mesh = Mesh_Load(shader_geometry_pass, "../../Examples/res/shapes/sphere.obj");
    ds.sphere_mesh     = sphere_mesh;

    // Texture for the cube
    Texture_t tex_cube = Texture_Create("../../Examples/res/textures/woodne_box.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    ds.tex_cube        = tex_cube;

    // Cube and Qwad
    VAO_t cube = Cube_Generate();
    VAO_t quad = Quad_Generate();

    ds.cube = cube;
    ds.quad = quad;

    // Setup gBuffer;
    GBuffer_t gBuf = GBuffer_Init(window.width, window.heigh);
    ds.gBuf        = gBuf;

    ds.scale = 0.1f;

    // Send light data to shaders...
    DirectionalLight_To_Shader(ds.dir_light, shader_directional_light_pass);
    // Spotlight_To_Shader();
    // Single we have different values, we will need to upload every draw call :(
    // PointLight_To_Shader(...);

    // Setting Texture ids for the lighting shaders, these will come from the gBuffer
    Shader_Bind(shader_geometry_pass);
    Shader_Uniform_Int(shader_geometry_pass, "diffuse0", 0);

    Shader_Bind(shader_directional_light_pass);
    Shader_Uniform_Int(shader_directional_light_pass, "gPositionMap", GBUFFER_TEXTURE_TYPE_POSITION);
    Shader_Uniform_Int(shader_directional_light_pass, "gNormalMap", GBUFFER_TEXTURE_TYPE_NORMAL);
    Shader_Uniform_Int(shader_directional_light_pass, "gColorMap", GBUFFER_TEXTURE_TYPE_DIFFUSE);

    // Shader_Uniform_Float(shader_directional_light_pass, "gMatSpecularIntensity", 0.0f);
    // Shader_Uniform_Float(shader_directional_light_pass, "gSpecularPower", 0.0f);
    Shader_Uniform_Vec2(shader_directional_light_pass, "gScreenSize", (vec2){(float)window.width, (float)window.heigh});

    Shader_Bind(shader_point_light_pass);
    Shader_Uniform_Int(shader_point_light_pass, "gPositionMap", GBUFFER_TEXTURE_TYPE_POSITION);
    Shader_Uniform_Int(shader_point_light_pass, "gNormalMap", GBUFFER_TEXTURE_TYPE_NORMAL);
    Shader_Uniform_Int(shader_point_light_pass, "gColorMap", GBUFFER_TEXTURE_TYPE_DIFFUSE);

    // Shader_Uniform_Float(shader_point_light_pass, "gMatSpecularIntensity", 0.0f);
    // Shader_Uniform_Float(shader_point_light_pass, "gSpecularPower", 0.0f);
    Shader_Uniform_Vec2(shader_point_light_pass, "gScreenSize", (vec2){(float)window.width, (float)window.heigh});
}

void Deferred3_Update()
{
    Camera_Inputs(&ds.cam);

    // This makes out gBuffer
    DSGeometryPass();

    // This will show the 4 parts all at once
    // DSLightPass();

    BeginLightPasses();

    DSPointLightsPass();

    DSDirectionalLightPass();
}

void Deferred3_OnExit()
{
    Camera_Print_Values(ds.cam);

    GBuffer_Destroy(&ds.gBuf);

    Mesh_Free(ds.sphere_mesh);

    Shader_Destroy(&ds.shader_geometry_pass);

    VAO_Destroy(ds.cube);
    VAO_Destroy(ds.quad);
}
