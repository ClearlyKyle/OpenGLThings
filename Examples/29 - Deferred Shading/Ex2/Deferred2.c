#include "Deferred2.h"

#include "GBuffer.h"

// Example from...
// https://ogldev.org/www/tutorial35/tutorial35.html

static struct DS
{
    Camera_t    cam;
    struct Mesh bag;

    GBuffer_t gBuf;
    Shader_t  shader_geometry_pass;
} ds;

static void DSGeometryPass()
{
    Shader_Bind(ds.shader_geometry_pass);
    Camera_View_Projection_To_Shader(ds.cam, ds.shader_geometry_pass, "camMatrix");

    GBuffer_BindForWriting(&ds.gBuf);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 model; // aka world
    glm_translate_make(model, (vec3){0.0f, 0.0f, 5.0f});
    glm_scale(model, (vec3){0.1f, 0.1f, 0.1f});
    // glm_rotate(model, (vec3){0.0f, m_scale, 0.0f});

    Shader_Uniform_Mat4(ds.shader_geometry_pass, "model", model);

    Mesh_Draw(ds.bag);
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

    GBuffer_SetReadBuffer(GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                      HalfWidth, 0, WINDOW_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Deferred2_Init()
{
    // Camera, dont forget this!!
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy(cam.position, (vec3){0.388311f, 0.006581f, 5.520222f});
    glm_vec3_copy(cam.orientation, (vec3){-0.522374f, 0.019198f, -0.852500f});
    cam.pitch = 1.100027f;
    cam.yaw   = -121.498100f;
    ds.cam    = cam;

    // Shaders
    Shader_t shader_geometry_pass = Shader_Create("../../Examples/29 - Deferred Shading/Ex2/geo_pass.vs",
                                                  "../../Examples/29 - Deferred Shading/Ex2/geo_pass.fs",
                                                  3,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "Position"},
                                                      {.index = 1, .name = "TexCoord"},
                                                      {.index = 2, .name = "Normal"},
                                                  });

    ds.shader_geometry_pass = shader_geometry_pass;

    // Load in models
    struct Mesh bag = Mesh_Load(shader_geometry_pass, "../../Examples/res/models/backpack/backpack.obj");
    ds.bag          = bag;

    // Setup gBuffer;
    GBuffer_t gBuf = GBuffer_Init(window.width, window.heigh);
    ds.gBuf        = gBuf;
}

void Deferred2_Update()
{
    Camera_Inputs(&ds.cam);
    DSGeometryPass();
    DSLightPass();
}

void Deferred2_OnExit()
{
    Camera_Print_Values(ds.cam);

    GBuffer_Destroy(&ds.gBuf);

    Mesh_Free(ds.bag);

    Shader_Destroy(&ds.shader_geometry_pass);
}
