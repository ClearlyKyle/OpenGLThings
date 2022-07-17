#include "IBL_Irradiance.h"

#include "stb/stb_image.h"

#define NUM_OF_LIGHTS 4

static struct IBL_Irradiance
{
    Camera_t cam;

    Shader_t pbrShader;
    Shader_t equirectangularToCubemapShader;
    Shader_t irradianceShader;
    Shader_t backgroundShader;

    VAO_t cube_vao;
    VAO_t sphere_vao;

    vec3 lightPositions[NUM_OF_LIGHTS];
    vec3 lightColours[NUM_OF_LIGHTS];

    GLuint irradianceMap;
    GLuint envCubemap;

    int   nrRows;
    int   nrColumns;
    float spacing;
} ibl;

void IBL_Irradiance_Init()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

    const int capture_width  = 512;
    const int capture_height = 512;

    // Camera
    ibl.cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);

    // Shaders
    ibl.pbrShader                      = Shader_Create("../../Examples/34 - PBR/IBL Irradiance/pbr.vs", "../../Examples/34 - PBR/IBL Irradiance/pbr.fs", 0, NULL);
    ibl.equirectangularToCubemapShader = Shader_Create("../../Examples/34 - PBR/IBL Irradiance/cubemap.vs", "../../Examples/34 - PBR/IBL Irradiance/equirectangular_to_cubemap.fs", 0, NULL);
    ibl.irradianceShader               = Shader_Create("../../Examples/34 - PBR/IBL Irradiance/cubemap.vs", "../../Examples/34 - PBR/IBL Irradiance/irradiance_convolution.fs", 0, NULL);
    ibl.backgroundShader               = Shader_Create("../../Examples/34 - PBR/IBL Irradiance/background.vs", "../../Examples/34 - PBR/IBL Irradiance/background.fs", 0, NULL);

    Shader_Bind(ibl.pbrShader);
    Shader_Uniform_Int(ibl.pbrShader, "irradianceMap", 0);
    Shader_Uniform_Vec3(ibl.pbrShader, "albedo", (vec3){0.5f, 0.0f, 0.0f});
    Shader_Uniform_Float(ibl.pbrShader, "ao", 1.0f);

    Shader_Bind(ibl.backgroundShader);
    Shader_Uniform_Int(ibl.backgroundShader, "environmentMap", 0);

    // Lights
    glm_vec3_copy((vec3){-10.0f, 10.0f, 10.0f}, ibl.lightPositions[0]);
    glm_vec3_copy((vec3){10.0f, 10.0f, 10.0f}, ibl.lightPositions[1]);
    glm_vec3_copy((vec3){-10.0f, -10.0f, 10.0f}, ibl.lightPositions[2]);
    glm_vec3_copy((vec3){10.0f, -10.0f, 10.0f}, ibl.lightPositions[3]);

    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, ibl.lightColours[0]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, ibl.lightColours[1]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, ibl.lightColours[2]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, ibl.lightColours[3]);

    ibl.nrRows    = 7;
    ibl.nrColumns = 7;
    ibl.spacing   = 2.5;

    // Cube
    ibl.cube_vao = Cube_Generate();

    // Sphere
    ibl.sphere_vao = Sphere_Generate();

    // PBR : Setup Framebuffer
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, capture_width, capture_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;

    const char *hdr_image_path = "../../Examples/res/textures/hdr/newport_loft.hdr";
    float      *image_data     = stbi_loadf(hdr_image_path, &width, &height, &nrComponents, 0);

    GLuint hdrTexture;
    if (image_data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        // note how we specify the texture's data value to be float
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image_data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(image_data);
    }
    else
    {
        fprintf(stderr, "Failed to load HDR image: %s\n", hdr_image_path);
        window.quit = true;
        return;
    }

    // PBR: Setup cubemap to render to and attach to framebuffer
    GLuint envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, capture_width, capture_height, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // PBR: Set up projection and view matrices for capturing data onto the 6 cubemap face directions
    mat4 captureProjection;
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 10.0f, captureProjection);

    mat4 captureViews[6] = {0};
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, captureViews[0]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){-1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, captureViews[1]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, captureViews[2]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, (vec3){0.0f, 0.0f, -1.0f}, captureViews[3]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, (vec3){0.0f, -1.0f, 0.0f}, captureViews[4]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, -1.0f}, (vec3){0.0f, -1.0f, 0.0f}, captureViews[5]);

    // PBR: convert HDR equirectangular environment map to cubemap equivalent
    Shader_Bind(ibl.equirectangularToCubemapShader);
    Shader_Uniform_Int(ibl.equirectangularToCubemapShader, "equirectangularMap", 0);
    Shader_Uniform_Mat4(ibl.equirectangularToCubemapShader, "projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, capture_width, capture_height); // don't forget to configure the viewport to the capture dimensions.

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        Shader_Uniform_Mat4(ibl.equirectangularToCubemapShader, "view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cube_Draw(ibl.cube_vao);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // PBR: Create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    GLuint irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // PBR: solve diffuse integral by convolution to create an irradiance (cube)map.
    Shader_Bind(ibl.irradianceShader);
    Shader_Uniform_Int(ibl.irradianceShader, "environmentMap", 0);
    Shader_Uniform_Mat4(ibl.irradianceShader, "projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        Shader_Uniform_Mat4(ibl.irradianceShader, "view", captureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cube_Draw(ibl.cube_vao);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Static shader values
    Shader_Bind(ibl.pbrShader);
    Shader_Uniform_Mat4(ibl.pbrShader, "projection", ibl.cam.projection);
    Shader_Bind(ibl.backgroundShader);
    Shader_Uniform_Mat4(ibl.backgroundShader, "projection", ibl.cam.projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    glViewport(0, 0, window.width, window.heigh);

    ibl.irradianceMap = irradianceMap;
    ibl.envCubemap    = envCubemap;
}

void IBL_Irradiance_Update()
{
    Camera_Inputs(&ibl.cam);

    // Render scene, supplying the convoluted irradiance map to the final shader.
    Shader_Bind(ibl.pbrShader);

    mat4 view;
    Camera_Get_View_Matrix(ibl.cam, view);

    Shader_Uniform_Mat4(ibl.pbrShader, "view", view);
    Shader_Uniform_Vec3(ibl.pbrShader, "camPos", ibl.cam.position);

    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.irradianceMap);

    // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
    mat4 model;
    for (int row = 0; row < ibl.nrRows; row++)
    {
        Shader_Uniform_Float(ibl.pbrShader, "metallic", (float)row / (float)ibl.nrRows);
        for (int col = 0; col < ibl.nrColumns; ++col)
        {
            // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
            // on direct lighting.
            Shader_Uniform_Float(ibl.pbrShader, "roughness", glm_clamp((float)col / (float)ibl.nrColumns, 0.05f, 1.0f));

            glm_translate_make(model, (vec3){
                                          (float)(col - (ibl.nrColumns / 2)) * ibl.spacing,
                                          (float)(row - (ibl.nrRows / 2)) * ibl.spacing,
                                          -2.0f});

            Shader_Uniform_Mat4(ibl.pbrShader, "model", model);

            Sphere_Draw(ibl.sphere_vao);
        }
    }

    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and
    // keeps the codeprint small.
    char buff[32];
    for (unsigned int i = 0; i < NUM_OF_LIGHTS; ++i)
    {
        vec3 newPos;
        // glm_vec3_add(ibl.lightPositions[i], (vec3){(float)sin(window.frame_time * 5.0) * 5.0f, 0.0f, 0.0f}, newPos);
        glm_vec3_copy(ibl.lightPositions[i], newPos);

        sprintf(buff, "lightPositions[%d]", i);
        Shader_Uniform_Vec3(ibl.pbrShader, buff, newPos);

        sprintf(buff, "lightColors[%d]", i);
        Shader_Uniform_Vec3(ibl.pbrShader, buff, ibl.lightColours[i]);

        glm_translate_make(model, newPos);
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

        Shader_Uniform_Mat4(ibl.pbrShader, "model", model);

        Sphere_Draw(ibl.sphere_vao);
    }

    // render skybox (render as last to prevent overdraw)
    Shader_Bind(ibl.backgroundShader);
    Shader_Uniform_Mat4(ibl.backgroundShader, "view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.envCubemap);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.irradianceMap); // display irradiance map

    Cube_Draw(ibl.cube_vao);
}

void IBL_Irradiance_OnExit()
{
    Camera_Print_Values(ibl.cam);

    Shader_Destroy(&ibl.pbrShader);
    Shader_Destroy(&ibl.equirectangularToCubemapShader);
    Shader_Destroy(&ibl.irradianceShader);
    Shader_Destroy(&ibl.backgroundShader);

    VAO_Destroy(ibl.cube_vao);
}
