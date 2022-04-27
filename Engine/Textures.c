#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION // use of stb functions once and for all
#include "stb/stb_image.h"

struct Texture Texture_Create(const char *path, GLenum texture_type, GLuint slot, GLenum format, GLenum pixel_type)
{
    assert((GL_TEXTURE0 + slot) < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

    struct Texture t;
    t.type = texture_type;
    t.slot = slot; // slot is used for "glActiveTexture(GL_TEXTURE0 + slot)"

    // Stores the width, height, and the number of color channels of the image
    int image_width, image_height, image_bpp;

    // Flips the image so it appears right side up
    stbi_set_flip_vertically_on_load(true);

    // Reads the image from a file and stores it in bytes
    unsigned char *image_bytes = stbi_load(path, &image_width, &image_height, &image_bpp, STBI_rgb_alpha);
    check_that(image_bytes != NULL, "[ERROR] (stbi_load) : %s\n", path);

    GLenum image_format;
    if (image_bpp == 1)
        image_format = GL_RED;
    else if (image_bpp == 3)
        image_format = GL_RGB;
    else if (image_bpp == 4)
        image_format = GL_RGBA;

    if (image_format != format)
    {
        fprintf(stderr, "Image format might not be correct %d != %d\n", image_format, format);
    }

    // Generates an OpenGL texture object
    glGenTextures(1, &t.ID);

    // OpenGL 4.5+
    // glBindTextureUnit(slot, t.ID); // same as glActiveTexture + glBindTexture

    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(texture_type, t.ID); // ID is used for "glBindTexture(..., ID)"

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Assigns the image to the OpenGL Texture object
    glTexImage2D(texture_type, 0, GL_RGBA, image_width, image_height, 0, format, pixel_type, image_bytes);

    // Generates MipMaps
    // glGenerateMipmap(texture_type);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(image_bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    // glBindTexture(texture_type, 0);

    return t;
}

// TODO : Remove this and update all examples.. enjoy
void Texture_Bind(const struct Texture t)
{
    glActiveTexture(GL_TEXTURE0 + t.slot);
    glBindTexture(t.type, t.ID);
}

void Texture_Bind_Slot(const struct Texture t, GLuint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(t.type, t.ID);
}

void Texture_Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture_Delete(const struct Texture t)
{
    glDeleteTextures(1, &t.ID);
}
