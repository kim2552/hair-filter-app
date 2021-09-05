//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_TEXTURE_H
#define CAMERA_HAIR_APP_TEXTURE_H

#include <GLES3/gl3.h>
#include<stb/stb_image.h>

#include"render/ShaderClass.h"

class Texture
{
public:
    GLuint ID;
    const char* type;
    GLuint unit;
    int m_width;
    int m_height;

    Texture();

    Texture(unsigned char* image, int len, const char* texType, GLuint slot);

    Texture(unsigned char* bytes, const char* texType, GLuint slot, int widthImg, int heightImg, int numColCh);

    // Update the texture image
    void updateTexture(unsigned char* bytes);
    // Assigns a texture unit to a texture
    void texUnit(Shader& shader, const char* uniform, GLuint unit);
    // Binds a texture
    void Bind();
    // Unbinds a texture
    void Unbind();
    // Deletes a texture
    void Delete();

private:
    void configureTexture(unsigned char* bytes, const char* texType, GLuint slot, int widthImg, int heightImg, int numColCh);
};

#endif //CAMERA_HAIR_APP_TEXTURE_H
