#include "Deferred4.h"

#include "GBuffer.h"

// Example from...
// https://ogldev.org/www/tutorial37/tutorial37.html

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
    Shader_t  shader_null;

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

    GBuffer_BindForGeomPass(&ds.gBuf);

    // Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

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
}

static float Calculate_Point_Light_Sphere(const PointLight_t *light)
{
    const float max_colour_channel = max(max(light->colour[0], light->colour[1]), light->colour[2]);

    const float ret = (-light->attenuation.linear + sqrtf(light->attenuation.linear * light->attenuation.linear - 4 * light->attenuation.exp * (light->attenuation.exp - 256.0f * max_colour_channel * light->diffuse_intensity)) /
                                                        (2 * light->attenuation.exp));
    return ret;
}

static void DSStencilPass(const unsigned int PointLightIndex)
{
    Shader_Bind(ds.shader_null);

    // Disable color/depth write and enable stencil
    GBuffer_BindForStencilPass(&ds.gBuf);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);

    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    const float BBoxScale = Calculate_Point_Light_Sphere(&ds.point_light[PointLightIndex]);

    mat4 model;

    glm_translate_make(model, ds.point_light[PointLightIndex].position);
    glm_scale(model, (vec3){BBoxScale, BBoxScale, BBoxScale});

    Shader_Uniform_Mat4(ds.shader_null, "model", model);
    Camera_View_Projection_To_Shader(ds.cam, ds.shader_null, "camMatrix");

    ds.sphere_mesh.shader = ds.shader_null;
    Mesh_Draw(ds.sphere_mesh);
}

static void DSPointLightPass(unsigned int PointLightIndex)
{
    GBuffer_BindForLightPass(&ds.gBuf);

    Shader_Bind(ds.shader_point_light_pass);
    Uniform_Vec3("gCameraPosition", ds.cam.position);

    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    const float BBoxScale = Calculate_Point_Light_Sphere(&ds.point_light[PointLightIndex]);

    mat4 model;

    glm_translate_make(model, ds.point_light[PointLightIndex].position);
    glm_scale(model, (vec3){BBoxScale, BBoxScale, BBoxScale});

    Shader_Uniform_Mat4(ds.shader_point_light_pass, "model", model);
    Camera_View_Projection_To_Shader(ds.cam, ds.shader_point_light_pass, "camMatrix");

    PointLight_To_Shader(ds.point_light[PointLightIndex], ds.shader_point_light_pass);

    ds.sphere_mesh.shader = ds.shader_point_light_pass;
    Mesh_Draw(ds.sphere_mesh);

    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
}

static void DSDirectionalLightPass()
{
    GBuffer_BindForLightPass(&ds.gBuf);

    Shader_Bind(ds.shader_directional_light_pass);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    Uniform_Vec3("gCameraPosition", ds.cam.position);
    Uniform_Mat4("model", model);
    Uniform_Mat4("camMatrix", model); // We dont need to transform the directional light?

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    Quad_Draw(ds.quad);

    glDisable(GL_BLEND);
}

static void DSFinalPass()
{
    GBuffer_BindForFinalPass(&ds.gBuf);

    glBlitFramebuffer(0, 0, window.width, window.heigh,
                      0, 0, window.width, window.heigh,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Deferred4_Init()
{
    // Makes the screen less "Hazy"
    Window_Clear_Colour(0.0f, 0.0f, 0.0f, 1.0f);

    // Camera, dont forget this!!
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-9.408190f, 4.230574f, -12.485654f}, cam.position);
    glm_vec3_copy((vec3){0.382649f, -0.158184f, 0.910252f}, cam.orientation);
    cam.pitch = -9.101479f;
    cam.yaw   = 67.199249f;

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
    Shader_t shader_geometry_pass = Shader_Create("../../Examples/29 - Deferred Shading/Ex4/geo_pass.vs",
                                                  "../../Examples/29 - Deferred Shading/Ex4/geo_pass.fs",
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

    Shader_t shader_null = Shader_Create("../../Examples/29 - Deferred Shading/Ex4/null_shader.vs",
                                         "../../Examples/29 - Deferred Shading/Ex4/null_shader.fs",
                                         1,
                                         (struct VertexAttribute[]){
                                             {.index = 0, .name = "Position"},
                                         });

    ds.shader_geometry_pass          = shader_geometry_pass;
    ds.shader_directional_light_pass = shader_directional_light_pass;
    ds.shader_point_light_pass       = shader_point_light_pass;
    ds.shader_null                   = shader_null;

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

    Shader_Uniform_Float(shader_directional_light_pass, "gMatSpecularIntensity", 0.0f);
    Shader_Uniform_Float(shader_directional_light_pass, "gSpecularPower", 0.0f);
    Shader_Uniform_Vec2(shader_directional_light_pass, "gScreenSize", (vec2){(float)window.width, (float)window.heigh});

    Shader_Bind(shader_point_light_pass);
    Shader_Uniform_Int(shader_point_light_pass, "gPositionMap", GBUFFER_TEXTURE_TYPE_POSITION);
    Shader_Uniform_Int(shader_point_light_pass, "gNormalMap", GBUFFER_TEXTURE_TYPE_NORMAL);
    Shader_Uniform_Int(shader_point_light_pass, "gColorMap", GBUFFER_TEXTURE_TYPE_DIFFUSE);

    Shader_Uniform_Float(shader_point_light_pass, "gMatSpecularIntensity", 0.0f);
    Shader_Uniform_Float(shader_point_light_pass, "gSpecularPower", 0.0f);
    Shader_Uniform_Vec2(shader_point_light_pass, "gScreenSize", (vec2){(float)window.width, (float)window.heigh});
}

void Deferred4_Update()
{
    Camera_Inputs(&ds.cam);

    // This makes out gBuffer
    GBuffer_StartFrame(&ds.gBuf);

    DSGeometryPass();

    // We need stencil to be enabled in the stencil pass to get the stencil buffer
    // updated and we also need it in the light pass because we render the light
    // only if the stencil passes.
    glEnable(GL_STENCIL_TEST);

    for (unsigned int i = 0; i < NUM_OF_POINT_LIGHTS; i++)
    {
        DSStencilPass(i);
        DSPointLightPass(i);
    }

    // The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.
    glDisable(GL_STENCIL_TEST);

    DSDirectionalLightPass();

    DSFinalPass();
}

void Deferred4_OnExit()
{
    Camera_Print_Values(ds.cam);

    GBuffer_Destroy(&ds.gBuf);

    Mesh_Free(ds.sphere_mesh);

    Shader_Destroy(&ds.shader_geometry_pass);

    VAO_Destroy(ds.cube);
    VAO_Destroy(ds.quad);
}
