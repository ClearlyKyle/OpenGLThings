#include "IBL_Specular.h"

#include "stb/stb_image.h"

#define NUM_OF_LIGHTS 4

static struct IBL_Specular
{
    Camera_t cam;

    Shader_t pbrShader;
    Shader_t equirectangularToCubemapShader;
    Shader_t irradianceShader;
    Shader_t prefilterShader;
    Shader_t brdfShader;
    Shader_t backgroundShader;

    VAO_t cube_vao;
    VAO_t sphere_vao;
    VAO_t quad_vao;

    GLuint irradianceMap;
    GLuint prefilterMap;
    GLuint brdfLUTTexture;
    GLuint envCubemap;

    vec3 lightPositions[NUM_OF_LIGHTS];
    vec3 lightColours[NUM_OF_LIGHTS];

    int   nrRows;
    int   nrColumns;
    float spacing;
} ibl;

void IBL_Specular_Init()
{
    glEnable(GL_DEPTH_TEST); // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // enable seamless cubemap sampling for lower mip levels in the pre-filter map.

    // Camera
    ibl.cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);

    // Cube
    ibl.cube_vao = Cube_Generate();

    // Sphere
    ibl.sphere_vao = Sphere_Generate();

    // Quad
    ibl.quad_vao = Quad_Generate();

    // Shaders
    ibl.pbrShader                      = Shader_Create("../../Examples/34 - PBR/IBL Specular/pbr.vs", "../../Examples/34 - PBR/IBL Specular/pbr.fs", 0, NULL);
    ibl.equirectangularToCubemapShader = Shader_Create("../../Examples/34 - PBR/IBL Specular/cubemap.vs", "../../Examples/34 - PBR/IBL Specular/equirectangular_to_cubemap.fs", 0, NULL);
    ibl.irradianceShader               = Shader_Create("../../Examples/34 - PBR/IBL Specular/cubemap.vs", "../../Examples/34 - PBR/IBL Specular/irradiance_convolution.fs", 0, NULL);
    ibl.prefilterShader                = Shader_Create("../../Examples/34 - PBR/IBL Specular/cubemap.vs", "../../Examples/34 - PBR/IBL Specular/prefilter.fs", 0, NULL);
    ibl.brdfShader                     = Shader_Create("../../Examples/34 - PBR/IBL Specular/brdf.vs", "../../Examples/34 - PBR/IBL Specular/brdf.fs", 0, NULL);
    ibl.backgroundShader               = Shader_Create("../../Examples/34 - PBR/IBL Specular/background.vs", "../../Examples/34 - PBR/IBL Specular/background.fs", 0, NULL);

    Shader_Bind(ibl.pbrShader);
    Shader_Uniform_Int(ibl.pbrShader, "irradianceMap", 0);
    Shader_Uniform_Int(ibl.pbrShader, "prefilterMap", 1);
    Shader_Uniform_Int(ibl.pbrShader, "brdfLUT", 2);
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

    // pbr: setup framebuffer

    const int capture_width  = 512;
    const int capture_height = 512;

    GLuint captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, capture_width, capture_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // PBR: Load the HDR environment map
    stbi_set_flip_vertically_on_load(true);

    int         width, height, nrComponents;
    const char *hdr_image_path = "../../Examples/res/textures/hdr/newport_loft.hdr";
    float      *data           = stbi_loadf(hdr_image_path, &width, &height, &nrComponents, 0);
    GLuint      hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        // Note how we specify the texture's data value to be float
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
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

    // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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

    // PBR: Create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    GLuint prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // PBR: Run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    Shader_Bind(ibl.prefilterShader);
    Shader_Uniform_Int(ibl.prefilterShader, "environmentMap", 0);
    Shader_Uniform_Mat4(ibl.prefilterShader, "projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    const unsigned int maxMipLevels = 5;
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        const unsigned int mipWidth  = (const unsigned int)(128 * pow(0.5, mip));
        const unsigned int mipHeight = (const unsigned int)(128 * pow(0.5, mip));

        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        const float roughness = (float)mip / (float)(maxMipLevels - 1);
        Shader_Uniform_Float(ibl.prefilterShader, "roughness", roughness);

        for (unsigned int i = 0; i < 6; ++i)
        {
            Shader_Uniform_Mat4(ibl.prefilterShader, "view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Cube_Draw(ibl.cube_vao);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // PBR: Generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    GLuint brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // Pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, capture_width, capture_height, 0, GL_RG, GL_FLOAT, 0);

    // Be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, capture_width, capture_height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, capture_width, capture_height);

    Shader_Bind(ibl.brdfShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Quad_Draw(ibl.quad_vao);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Static shader values
    Shader_Bind(ibl.pbrShader);
    Shader_Uniform_Mat4(ibl.pbrShader, "projection", ibl.cam.projection);
    Shader_Bind(ibl.backgroundShader);
    Shader_Uniform_Mat4(ibl.backgroundShader, "projection", ibl.cam.projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    glViewport(0, 0, window.width, window.heigh);

    ibl.irradianceMap  = irradianceMap;
    ibl.prefilterMap   = prefilterMap;
    ibl.brdfLUTTexture = brdfLUTTexture;
    ibl.envCubemap     = envCubemap;
}

void IBL_Specular_Update()
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
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ibl.brdfLUTTexture);

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

    // Render skybox (render as last to prevent overdraw)
    Shader_Bind(ibl.backgroundShader);
    Shader_Uniform_Mat4(ibl.backgroundShader, "view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.envCubemap);
    //  glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.irradianceMap); // display irradiance map
    // glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.prefilterMap); // display prefilter map

    Cube_Draw(ibl.cube_vao);

    // Render BRDF map to screen
    // Shader_Bind(ibl.brdfShader);
    // Quad_Draw(ibl.quad_vao);
}

void IBL_Specular_OnExit()
{
    Camera_Print_Values(ibl.cam);

    Shader_Destroy(&ibl.pbrShader);
    Shader_Destroy(&ibl.equirectangularToCubemapShader);
    Shader_Destroy(&ibl.irradianceShader);
    Shader_Destroy(&ibl.prefilterShader);
    Shader_Destroy(&ibl.brdfShader);
    Shader_Destroy(&ibl.backgroundShader);

    VAO_Destroy(ibl.cube_vao);
    VAO_Destroy(ibl.sphere_vao);
    VAO_Destroy(ibl.quad_vao);

    if (ibl.irradianceMap != 0)
        glDeleteBuffers(1, &ibl.irradianceMap);

    if (ibl.prefilterMap != 0)
        glDeleteBuffers(1, &ibl.prefilterMap);

    if (ibl.brdfLUTTexture != 0)
        glDeleteBuffers(1, &ibl.brdfLUTTexture);

    if (ibl.envCubemap != 0)
        glDeleteBuffers(1, &ibl.envCubemap);
}
