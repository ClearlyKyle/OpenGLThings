#include "CameraExample.h"

static struct CameraExample cam_example;

void CameraExample_Init()
{
    // Vertices coordinates
    GLfloat vertices[] =
        {//     COORDINATES     /        COLORS      /   TexCoord  //
         -0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
         -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
         0.0f, 0.8f, 0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f};

    // Indices for vertices order
    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3,
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4};

    cam_example.numer_of_indicies = sizeof(indices) / sizeof(GLuint);

    // Generates Shader object using shaders defualt.vert and default.frag
    struct Shader shader = Shader_Create(
        "../../Examples/shaders/5/camera_example.vs",
        "../../Examples/shaders/5/camera_example.fs",
        3,
        (struct VertexAttribute[]){
            {.index = 0, .name = "aPos"},
            {.index = 1, .name = "aColor"},
            {.index = 2, .name = "aTex"}});

    cam_example.shader = shader;

    // Generates Vertex Array Object and binds it
    struct VAO vao = VAO_Create();
    VAO_Bind(vao);
    cam_example.vao = vao;

    // Generates Vertex Buffer Object and links it to vertices
    struct VBO vbo = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(vbo, sizeof(vertices), (const GLvoid *)vertices);

    struct EBO ebo = EBO_Create();
    EBO_Buffer(ebo, sizeof(indices), (void *)indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO_Attr(vao, vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)0);
    VAO_Attr(vao, vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
    VAO_Attr(vao, vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));

    // Texture
    const char *file_path = "../../Examples/res/textures/brick.png";
    struct Texture tex    = Texture_Create(file_path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    cam_example.tex       = tex;

    Shader_Uniform_Texture2D(shader, "tex0", tex);

    // Camera
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 100.0f);
    cam_example.cam   = cam;

    // EBO_Unbind();
}

void CameraExample_Update()
{
    Shader_Bind(cam_example.shader);

    // Handles camera inputs
    Camera_Inputs(&cam_example.cam);

    // Updates and exports the camera matrix to the Vertex Shader
    Camera_View_Projection_To_Shader(cam_example.cam, cam_example.shader, "camMatrix");

    // Binds texture so that is appears in rendering
    Texture_Bind(cam_example.tex);
    VAO_Bind(cam_example.vao);

    // printf("Camera Position : (%f, %f, %f)\n", cam_example.cam.position[0], cam_example.cam.position[1], cam_example.cam.position[2]);
    printf("Camera Orientation : (%f, %f, %f)\n", cam_example.cam.orientation[0], cam_example.cam.orientation[1], cam_example.cam.orientation[2]);

    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, cam_example.numer_of_indicies, GL_UNSIGNED_INT, 0);
}

void CameraExample_OnExit()
{
    Shader_Destroy(&cam_example.shader);
    Texture_Delete(cam_example.tex);
    VAO_Destroy(cam_example.vao);
}