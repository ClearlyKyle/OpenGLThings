#include "PBRLighting.h"

#define NUM_OF_LIGHTS 4

static struct PBRLighting
{
    Camera_t cam;

    Shader_t shader;

    vec3 lightPositions[NUM_OF_LIGHTS];
    vec3 lightColours[NUM_OF_LIGHTS];

    int   nrRows;
    int   nrColumns;
    float spacing;

    VAO_t sphere_vao;
} pbr;

void PBRLighting_Init()
{
    glEnable(GL_DEPTH_TEST);

    // Camera
    pbr.cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);

    // Shaders
    pbr.shader = Shader_Create("../../Examples/34 - PBR/Lighting/PBRLighting.vs",
                               "../../Examples/34 - PBR/Lighting/PBRLighting.fs",
                               3,
                               (struct VertexAttribute[]){
                                   {.index = 0, .name = "aPos"},
                                   {.index = 1, .name = "aNormal"},
                                   {.index = 2, .name = "aTexCoords"},
                               });

    Shader_Bind(pbr.shader);
    Shader_Uniform_Vec3(pbr.shader, "albedo", (vec3){0.5f, 0.0f, 0.0f});
    Shader_Uniform_Float(pbr.shader, "ao", 1.0f);

    // Lights
    glm_vec3_copy((vec3){-10.0f, 10.0f, 10.0f}, pbr.lightPositions[0]);
    glm_vec3_copy((vec3){10.0f, 10.0f, 10.0f}, pbr.lightPositions[1]);
    glm_vec3_copy((vec3){-10.0f, -10.0f, 10.0f}, pbr.lightPositions[2]);
    glm_vec3_copy((vec3){10.0f, -10.0f, 10.0f}, pbr.lightPositions[3]);

    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.lightColours[0]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.lightColours[1]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.lightColours[2]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.lightColours[3]);

    pbr.nrRows    = 7;
    pbr.nrColumns = 7;
    pbr.spacing   = 2.5;

    // Sphere
    pbr.sphere_vao = Sphere_Generate();
}

void PBRLighting_Update()
{
    Camera_Inputs(&pbr.cam);

    Shader_Bind(pbr.shader);
    Shader_Uniform_Vec3(pbr.shader, "camPos", pbr.cam.position);

    Camera_View_Projection_To_Shader(pbr.cam, pbr.shader, "view");

    // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
    mat4 model;
    for (int row = 0; row < pbr.nrRows; ++row)
    {
        for (int col = 0; col < pbr.nrColumns; ++col)
        {
            // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
            // on direct lighting.
            Shader_Uniform_Float(pbr.shader, "roughness", glm_clamp((float)col / (float)pbr.nrColumns, 0.05f, 1.0f));

            glm_translate_make(model, (vec3){
                                          (float)(col - (pbr.nrColumns / 2)) * pbr.spacing,
                                          (float)(row - (pbr.nrRows / 2)) * pbr.spacing,
                                          0.0f});

            Shader_Uniform_Mat4(pbr.shader, "model", model);

            Sphere_Draw(pbr.sphere_vao);
        }
    }

    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and
    // keeps the codeprint small.
    char buff[32];
    for (unsigned int i = 0; i < NUM_OF_LIGHTS; ++i)
    {
        vec3 newPos;
        // glm_vec3_add(pbr.lightPositions[i], (vec3){(float)sin(window.frame_time * 5.0) * 5.0f, 0.0f, 0.0f}, newPos);
        glm_vec3_copy(pbr.lightPositions[i], newPos);

        sprintf(buff, "lightPositions[%d]", i);
        Shader_Uniform_Vec3(pbr.shader, buff, newPos);

        sprintf(buff, "lightColours[%d]", i);
        Shader_Uniform_Vec3(pbr.shader, buff, pbr.lightColours[i]);

        glm_translate_make(model, newPos);
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

        Shader_Uniform_Mat4(pbr.shader, "model", model);

        Sphere_Draw(pbr.sphere_vao);
    }
}

void PBRLighting_OnExit()
{
    Camera_Print_Values(pbr.cam);

    Shader_Destroy(&pbr.shader);
    VAO_Destroy(pbr.sphere_vao);
}
