//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_EBO_H
#define CAMERA_HAIR_APP_EBO_H

#include <GLES3/gl3.h>
#include<vector>

#include "globals.h"

class EBO
{
public:
    // ID reference of Elements Buffer Object
    GLuint ID;
    // Constructor that generates a Elements Buffer Object and links it to indices
    EBO(std::vector<GLuint>& indices);

    // Binds the EBO
    void Bind();
    // Unbinds the EBO
    void Unbind();
    // Deletes the EBO
    void Delete();
};

#endif //CAMERA_HAIR_APP_EBO_H
