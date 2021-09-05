//
// Created by jooho on 2021-08-26.
//

#include"Texture.h"

Texture::Texture():
    ID(0),
    type(NULL),
    unit(0),
    m_width(0),
    m_height(0)
{};

void Texture::configureTexture(unsigned char* bytes, const char* texType, GLuint slot, int widthImg, int heightImg, int numColCh)
{
    // Assigns the type of the texture ot the texture object
    type = texType;

    // Generates an OpenGL texture object
    glGenTextures(1, &ID);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);

    // Save the width and height
    m_width = widthImg;
    m_height = heightImg;

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Fixes alignment issues when unpacking the image: https://stackoverflow.com/questions/30278833/opengl-texturing-some-jpgs-are-being-distorted-in-a-strange-way
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

    // Check what type of color channels the texture has and load it accordingly
    if (numColCh == 4)
        glTexImage2D
                (
                        GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        widthImg,
                        heightImg,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        bytes
                );
    else if (numColCh == 3)
        glTexImage2D
                (
                        GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        widthImg,
                        heightImg,
                        0,
                        GL_RGB,
                        GL_UNSIGNED_BYTE,
                        bytes
                );
    else if (numColCh == 1)
        glTexImage2D
            (
                    GL_TEXTURE_2D,
                    0,
                    GL_LUMINANCE,
                    widthImg,
                    heightImg,
                    0,
                    GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    bytes
            );
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    // Generates MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Interpolation for Mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(unsigned char* image,int len, const char* texType, GLuint slot)
{
    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    stbi_set_flip_vertically_on_load(true);
    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = stbi_load_from_memory(image, len, &widthImg, &heightImg, &numColCh, 0);

    configureTexture(bytes, texType, slot, widthImg, heightImg, numColCh);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);
}

Texture::Texture(unsigned char* bytes, const char* texType, GLuint slot, int widthImg, int heightImg, int numColCh)
{
    configureTexture(bytes, texType, slot, widthImg, heightImg, numColCh);
}

void Texture::updateTexture(unsigned char *bytes)
{
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_width, (GLsizei)m_height, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, bytes);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    // Gets the location of the uniform
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    // Shader needs to be activated before changing the value of a uniform
    shader.Activate();
    // Sets the value of the uniform
    glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}