#include "features/skybox.h"

#include "stb/stb_image.h"

Skybox_t Skybox_Create(struct Shader shader, const char *cube_map_face_images[6])
{
    const GLfloat skybox_verticies[] =
        {
            //   Coordinates
            -1.0f, -1.0f, 1.0f,  //        7--------6
            1.0f, -1.0f, 1.0f,   //       /|       /|
            1.0f, -1.0f, -1.0f,  //      4--------5 |
            -1.0f, -1.0f, -1.0f, //      | |      | |
            -1.0f, 1.0f, 1.0f,   //      | 3------|-2
            1.0f, 1.0f, 1.0f,    //      |/       |/
            1.0f, 1.0f, -1.0f,   //      0--------1
            -1.0f, 1.0f, -1.0f};

    const GLuint skybox_indicies[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3};

    Skybox_t skybox;
    skybox.shader = shader;

    Shader_Bind(skybox.shader);
    glUniform1i(glGetUniformLocation(skybox.shader.shader_id, "skybox"), 0);

    // Create VAO, VBO, and EBO for the skybox
    GLuint skybox_VAO, skybox_VBO, skybox_EBO;
    glGenVertexArrays(1, &skybox_VAO);

    glGenBuffers(1, &skybox_VBO);
    glGenBuffers(1, &skybox_EBO);

    glBindVertexArray(skybox_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_verticies), &skybox_verticies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indicies), &skybox_indicies, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *)(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    skybox.VAO = skybox_VAO;

    // All the faces of the cubemap (make sure they are in this exact order)
    // const char *cube_map_face_images[6] =
    //    {
    //        "../../Examples/res/textures/skybox/right.jpg",
    //        "../../Examples/res/textures/skybox/left.jpg",
    //        "../../Examples/res/textures/skybox/top.jpg",
    //        "../../Examples/res/textures/skybox/bottom.jpg",
    //        "../../Examples/res/textures/skybox/front.jpg",
    //        "../../Examples/res/textures/skybox/back.jpg"};

    // Creates the cubemap texture object
    GLuint cubemap_texture;
    glGenTextures(1, &cubemap_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // This might help with seams on some systems
    skybox.texture = cubemap_texture;

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        stbi_set_flip_vertically_on_load(false);

        int            width, height, bpp;
        unsigned char *image_data = stbi_load(cube_map_face_images[i], &width, &height, &bpp, 0);

        check_that(image_data, "Failed to load texture: %s\n", cube_map_face_images[i]);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     image_data);

        stbi_image_free(image_data);
    }

    fprintf(stderr, "Skybox Created\n");
    return skybox;
}

Skybox_t Skybox_Create_Default()
{
    struct Shader skybox_shader = Shader_Create("../../Examples/16 - Instancing/skybox.vs",
                                                "../../Examples/16 - Instancing/skybox.fs",
                                                1,
                                                (struct VertexAttribute[]){
                                                    {.index = 0, .name = "inPos"}});

    // All the faces of the cubemap (make sure they are in this exact order)
    const char *cube_map_face_images[6] =
        {
            "../../Examples/res/textures/skybox/right.jpg",
            "../../Examples/res/textures/skybox/left.jpg",
            "../../Examples/res/textures/skybox/top.jpg",
            "../../Examples/res/textures/skybox/bottom.jpg",
            "../../Examples/res/textures/skybox/front.jpg",
            "../../Examples/res/textures/skybox/back.jpg"};

    Skybox_t skybox = Skybox_Create(skybox_shader, cube_map_face_images);
    return skybox;
}

void Skybox_Draw(Skybox_t skybox, Camera_t cam)
{
    // SKYBOX DRAW START ------------------------
    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    Shader_Bind(skybox.shader);

    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection  = GLM_MAT4_IDENTITY_INIT;
    // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
    // The last row and column affect the translation of the skybox (which we don't want to affect)

    Camera_Get_View_Matrix(cam, view_matrix);
    view_matrix[0][3]  = 0.0f;
    view_matrix[0][7]  = 0.0f;
    view_matrix[0][11] = 0.0f;
    view_matrix[0][15] = 0.0f;
    view_matrix[3][0]  = 0.0f;
    view_matrix[3][1]  = 0.0f;
    view_matrix[3][2]  = 0.0f;
    view_matrix[3][3]  = 0.0f;

    glm_perspective(glm_rad(45.0f), (float)window.width / window.heigh, 0.1f, 100.0f, projection);

    Shader_Uniform_Mat4(skybox.shader, "view", view_matrix);
    Shader_Uniform_Mat4(skybox.shader, "projection", projection);

    // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
    // where an object is present (a depth of 1.0f will always fail against any object's depth value)
    glBindVertexArray(skybox.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);

    // SKYBOX DRAW END --------------------------
}

void Skybox_Free(Skybox_t skybox)
{
    Shader_Destroy(&skybox.shader);
    glDeleteVertexArrays(1, &skybox.VAO);
    glDeleteTextures(1, &skybox.texture);
}