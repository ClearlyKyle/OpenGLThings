#include "Deferred.h"

#include <time.h>

#define NUM_OF_OBJECTS 9
#define NR_LIGHTS 32

static struct Deferred
{
    Camera_t cam;

    GLuint gBuffer;
    GLuint gPosition;
    GLuint gNormal;
    GLuint gAlbedoSpec;

    Shader_t shader_geometry_pass;
    Shader_t shader_lighting_pass;
    Shader_t shader_light_box;

    vec3 object_positions[NUM_OF_OBJECTS];
    vec3 lightPositions[NR_LIGHTS];
    vec3 lightColors[NR_LIGHTS];

    VAO_t cube_VAO;
    VAO_t quad_VAO;

    struct Mesh model;
} def;

VAO_t Cube_Generate()
{
    const GLfloat vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    VAO_t VAO = VAO_Create();
    VBO_t VBO = VBO_Create(GL_ARRAY_BUFFER);

    VBO_Buffer(VBO, sizeof(vertices), (const GLvoid *)vertices);

    VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(VAO, VBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    return VAO;
}

VAO_t Quad_Generate()
{
    const GLfloat vertices[] = {
        // positions      // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  //
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   //
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //
    };

    VAO_t VAO = VAO_Create();
    VBO_t VBO = VBO_Create(GL_ARRAY_BUFFER);

    VBO_Buffer(VBO, sizeof(vertices), (const GLvoid *)vertices);

    VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(VAO, VBO, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    return VAO;
};

void Deferred_Init()
{
    // Camera, dont forget this!!
    const Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    def.cam            = cam;

    // Shaders
    Shader_t shader_geometry_pass = Shader_Create("../../Examples/29 - Deferred Shading/g_buffer.vs",
                                                  "../../Examples/29 - Deferred Shading/g_buffer.fs",
                                                  3,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aNormal"},
                                                      {.index = 2, .name = "aTexCoords"},
                                                  });

    Shader_t shader_lighting_pass = Shader_Create("../../Examples/29 - Deferred Shading/deferred_shading.vs",
                                                  "../../Examples/29 - Deferred Shading/deferred_shading.fs",
                                                  2,
                                                  (struct VertexAttribute[]){
                                                      {.index = 0, .name = "aPos"},
                                                      {.index = 1, .name = "aTexCoords"},
                                                  });

    Shader_t shader_light_box = Shader_Create("../../Examples/29 - Deferred Shading/deferred_light_box.vs",
                                              "../../Examples/29 - Deferred Shading/deferred_light_box.fs",
                                              3,
                                              (struct VertexAttribute[]){
                                                  {.index = 0, .name = "aPos"},
                                                  {.index = 1, .name = "aNormal"},
                                                  {.index = 2, .name = "aTexCoords"},
                                              });

    def.shader_geometry_pass = shader_geometry_pass;
    def.shader_lighting_pass = shader_lighting_pass;
    def.shader_light_box     = shader_light_box;

    // Load in models
    struct Mesh bag = Mesh_Load(shader_geometry_pass, "../../Examples/res/models/backpack/backpack.obj");
    def.model       = bag;

    glm_vec3_copy((vec3){-3.0, -0.5, -3.0}, def.object_positions[0]);
    glm_vec3_copy((vec3){0.0, -0.5, -3.0}, def.object_positions[1]);
    glm_vec3_copy((vec3){3.0, -0.5, -3.0}, def.object_positions[2]);
    glm_vec3_copy((vec3){-3.0, -0.5, 0.0}, def.object_positions[3]);
    glm_vec3_copy((vec3){0.0, -0.5, 0.0}, def.object_positions[4]);
    glm_vec3_copy((vec3){3.0, -0.5, 0.0}, def.object_positions[5]);
    glm_vec3_copy((vec3){-3.0, -0.5, 3.0}, def.object_positions[6]);
    glm_vec3_copy((vec3){0.0, -0.5, 3.0}, def.object_positions[7]);
    glm_vec3_copy((vec3){3.0, -0.5, 3.0}, def.object_positions[8]);

    // Configure g-buffer framebuffer
    GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    GLuint gPosition, gNormal, gAlbedoSpec;

    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.width, window.heigh, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.width, window.heigh, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width, window.heigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    // create and attach depth buffer (renderbuffer)
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.width, window.heigh);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "Framebuffer not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    def.gBuffer     = gBuffer;
    def.gPosition   = gPosition;
    def.gNormal     = gNormal;
    def.gAlbedoSpec = gAlbedoSpec;

    // Lighting info
    srand((unsigned int)time(NULL));

    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = (float)(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float yPos = (float)(((rand() % 100) / 100.0) * 6.0 - 4.0);
        float zPos = (float)(((rand() % 100) / 100.0) * 6.0 - 3.0);
        glm_vec3_copy((vec3){xPos, yPos, zPos}, def.lightPositions[i]);

        // also calculate random color
        float rColor = (float)(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float gColor = (float)(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float bColor = (float)(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        glm_vec3_copy((vec3){rColor, gColor, bColor}, def.lightColors[i]);
    }

    // Shader configuration
    Shader_Bind(shader_lighting_pass);
    Shader_Uniform_Int(shader_lighting_pass, "gPosition", 0);
    Shader_Uniform_Int(shader_lighting_pass, "gNormal", 1);
    Shader_Uniform_Int(shader_lighting_pass, "gAlbedoSpec", 2);

    // Cube and Quad
    def.cube_VAO = Cube_Generate();
    def.quad_VAO = Quad_Generate();
}

void Deferred_Update()
{
    Camera_Inputs(&def.cam);

    // 1. geometry pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, def.gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // glm::mat4 view       = camera.GetViewMatrix();
    // glm::mat4 model      = glm::mat4(1.0f);
    // shaderGeometryPass.use();
    // shaderGeometryPass.setMat4("projection", projection);
    // shaderGeometryPass.setMat4("view", view);

    mat4 model;
    Shader_Bind(def.shader_geometry_pass);
    Camera_View_Projection_To_Shader(def.cam, def.shader_geometry_pass, "camMatrix");

    for (unsigned int i = 0; i < 9; i++)
    {
        glm_translate_make(model, def.object_positions[i]);
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

        // Precompute Model matrices out of here
        Shader_Uniform_Mat4(def.shader_geometry_pass, "model", model);
        Mesh_Draw(def.model);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader_Bind(def.shader_lighting_pass);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, def.gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, def.gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, def.gAlbedoSpec);

    // send light relevant uniforms
    char buff[32];
    // update attenuation parameters and calculate radius
    const float linear    = 0.7f;
    const float quadratic = 1.8f;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        sprintf(buff, "lights[%d].Position", i);
        Shader_Uniform_Vec3(def.shader_lighting_pass, buff, def.lightPositions[i]);

        sprintf(buff, "lights[%d].Color", i);
        Shader_Uniform_Vec3(def.shader_lighting_pass, buff, def.lightColors[i]);

        sprintf(buff, "lights[%d].Linear", i);
        Shader_Uniform_Float(def.shader_lighting_pass, buff, linear);

        sprintf(buff, "lights[%d].Quadratic", i);
        Shader_Uniform_Float(def.shader_lighting_pass, buff, quadratic);
    }

    Shader_Uniform_Vec3(def.shader_lighting_pass, "viewPos", def.cam.position);

    // finally render quad
    VAO_Bind(def.quad_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, def.gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, window.width, window.heigh, 0, 0, window.width, window.heigh, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. render lights on top of scene
    Shader_Bind(def.shader_light_box);
    Camera_View_Projection_To_Shader(def.cam, def.shader_light_box, "camMatrix");
    // Shader_Uniform_Mat4(def.shader_light_box, "projection", projection);
    // Shader_Uniform_Mat4(def.shader_light_box, "view", view);

    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        glm_translate_make(model, def.object_positions[i]);
        glm_scale(model, (vec3){0.125f, 0.125f, 0.125f});

        Shader_Uniform_Mat4(def.shader_light_box, "model", model);
        Shader_Uniform_Vec3(def.shader_light_box, "lightColor", def.lightColors[i]);

        // Render cube
        VAO_Bind(def.cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void Deferred_OnExit()
{
    Camera_Print_Values(def.cam);

    glDeleteFramebuffers(1, &def.gBuffer);
    glDeleteFramebuffers(1, &def.gPosition);
    glDeleteFramebuffers(1, &def.gNormal);
    glDeleteFramebuffers(1, &def.gAlbedoSpec);

    Shader_Destroy(&def.shader_geometry_pass);
    Shader_Destroy(&def.shader_lighting_pass);
    Shader_Destroy(&def.shader_light_box);

    VAO_Destroy(def.cube_VAO);
    VAO_Destroy(def.quad_VAO);

    Mesh_Free(def.model);
}
