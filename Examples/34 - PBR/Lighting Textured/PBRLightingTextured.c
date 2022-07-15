#include "PBRLightingTextured.h"

#define NUM_OF_LIGHTS 4

static struct PBRLightingTextured
{
    Camera_t cam;

    Shader_t shader;

    VAO_t sphere_vao;

    int   nrRows;
    int   nrColumns;
    float spacing;

    vec3 light_positions[NUM_OF_LIGHTS];
    vec3 light_colours[NUM_OF_LIGHTS];

    Texture_t albedo;
    Texture_t normal;
    Texture_t metallic;
    Texture_t roughness;
    Texture_t ao;
} pbr;

void PBRLightingTextured_Init()
{
    glEnable(GL_DEPTH_TEST);

    // Camera
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-6.122798f, 2.797147f, 3.666487f}, cam.position);
    glm_vec3_copy((vec3){0.815620f, -0.078462f, -0.573243f}, cam.orientation);
    cam.pitch = -4.500177f;
    cam.yaw   = -35.100727f;

    pbr.cam = cam;

    // Shader
    pbr.shader = Shader_Create("../../Examples/34 - PBR/Lighting Textured/PBRLightingTextured.vs",
                               "../../Examples/34 - PBR/Lighting Textured/PBRLightingTextured.fs",
                               3,
                               (struct VertexAttribute[]){
                                   {.index = 0, .name = "aPos"},
                                   {.index = 1, .name = "aNormal"},
                                   {.index = 2, .name = "aTexCoords"},
                               });

    Shader_Bind(pbr.shader);
    Shader_Uniform_Int(pbr.shader, "albedoMap", 0);
    Shader_Uniform_Int(pbr.shader, "normalMap", 1);
    Shader_Uniform_Int(pbr.shader, "metallicMap", 2);
    Shader_Uniform_Int(pbr.shader, "roughnessMap", 3);
    Shader_Uniform_Int(pbr.shader, "aoMap", 4);

    pbr.albedo    = Texture_Create("../../Examples/res/textures/pbr/rusted_iron/albedo.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    pbr.normal    = Texture_Create("../../Examples/res/textures/pbr/rusted_iron/normal.png", GL_TEXTURE_2D, 1, GL_RGB, GL_UNSIGNED_BYTE);
    pbr.metallic  = Texture_Create("../../Examples/res/textures/pbr/rusted_iron/metallic.png", GL_TEXTURE_2D, 2, GL_RED, GL_UNSIGNED_BYTE);
    pbr.roughness = Texture_Create("../../Examples/res/textures/pbr/rusted_iron/roughness.png", GL_TEXTURE_2D, 3, GL_RED, GL_UNSIGNED_BYTE);
    pbr.ao        = Texture_Create("../../Examples/res/textures/pbr/rusted_iron/ao.png", GL_TEXTURE_2D, 4, GL_RGB, GL_UNSIGNED_BYTE);

    // Lights
    glm_vec3_copy((vec3){-10.0f, 10.0f, 10.0f}, pbr.light_positions[0]);
    glm_vec3_copy((vec3){10.0f, 10.0f, 10.0f}, pbr.light_positions[1]);
    glm_vec3_copy((vec3){-10.0f, -10.0f, 10.0f}, pbr.light_positions[2]);
    glm_vec3_copy((vec3){10.0f, -10.0f, 10.0f}, pbr.light_positions[3]);

    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.light_colours[0]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.light_colours[1]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.light_colours[2]);
    glm_vec3_copy((vec3){300.0f, 300.0f, 300.0f}, pbr.light_colours[3]);

    pbr.nrRows    = 7;
    pbr.nrColumns = 7;
    pbr.spacing   = 2.5;

    // Sphere
    pbr.sphere_vao = Sphere_Generate();
}

void PBRLightingTextured_Update()
{
    Camera_Inputs(&pbr.cam);

    Shader_Bind(pbr.shader);
    Shader_Uniform_Vec3(pbr.shader, "camPos", pbr.cam.position);

    Camera_View_Projection_To_Shader(pbr.cam, pbr.shader, "view");

    Texture_Bind(pbr.albedo);
    Texture_Bind(pbr.normal);
    Texture_Bind(pbr.metallic);
    Texture_Bind(pbr.roughness);
    Texture_Bind(pbr.ao);

    // render rows*column number of spheres with material properties defined by textures (they all have the same material properties)
    mat4 model;

    for (int row = 0; row < pbr.nrRows; ++row)
    {
        for (int col = 0; col < pbr.nrColumns; ++col)
        {
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
        // vec3 newPos;
        // glm_vec3_add(pbr.light_positions[i], (vec3){(float)sin(window.frame_time * 5.0) * 5.0f, 0.0f, 0.0f}, newPos);
        //  newPos      = pbr.light_positions[i];

        sprintf(buff, "lightPositions[%d]", i);
        Shader_Uniform_Vec3(pbr.shader, buff, pbr.light_positions[i]);

        sprintf(buff, "lightColours[%d]", i);
        Shader_Uniform_Vec3(pbr.shader, buff, pbr.light_colours[i]);

        glm_translate_make(model, pbr.light_positions[i]);
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

        Shader_Uniform_Mat4(pbr.shader, "model", model);

        Sphere_Draw(pbr.sphere_vao);
    }
}

void PBRLightingTextured_OnExit()
{
    Camera_Print_Values(pbr.cam);

    Shader_Destroy(&pbr.shader);
    VAO_Destroy(pbr.sphere_vao);

    Texture_Delete(pbr.albedo);
    Texture_Delete(pbr.normal);
    Texture_Delete(pbr.metallic);
    Texture_Delete(pbr.roughness);
    Texture_Delete(pbr.ao);
}
