#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION // use of stb functions once and for all
#include "stb/stb_image.h"

struct Texture Texture_Create(const char *path, GLenum texture_type, GLenum slot, GLenum format, GLenum pixel_type)
{
    struct Texture t;
    t.type = texture_type;

    // Stores the width, height, and the number of color channels of the image
    int image_width, image_height, image_bpp;

    // Flips the image so it appears right side up
    stbi_set_flip_vertically_on_load(true);

    // Reads the image from a file and stores it in bytes
    unsigned char *image_bytes = stbi_load(path, &image_width, &image_height, &image_bpp, STBI_rgb_alpha);

    // Generates an OpenGL texture object
    glGenTextures(1, &t.ID);

    // Assigns the texture to a Texture Unit
    glActiveTexture(slot);
    glBindTexture(texture_type, t.ID);

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Assigns the image to the OpenGL Texture object
    glTexImage2D(texture_type, 0, GL_RGBA, image_width, image_height, 0, format, pixel_type, image_bytes);
    // Generates MipMaps
    //glewExperimental = GL_TRUE;
    glGenerateMipmap(texture_type);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(image_bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(texture_type, 0);

    return t;
}

// Move this to Shder file?
void Texture_Uniform(const struct Shader shader, const char *uniform, GLuint unit)
{
    // Gets the location of the uniform
    const GLuint texUni = glGetUniformLocation(shader.shader_id, uniform);

    // Shader needs to be activated before changing the value of a uniform
    Shader_Bind(shader);

    // Sets the value of the uniform
    glUniform1i(texUni, unit);
}

void Texture_Bind(const struct Texture t)
{
    glBindTexture(t.type, t.ID);
}

void Texture_Unbind()
// void Texture_Unbind(const struct Texture t)
{
    // glBindTexture(t.type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture_Delete(const struct Texture t)
{
    glDeleteTextures(1, &t.ID);
}
