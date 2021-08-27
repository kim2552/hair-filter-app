//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_VBO_H
#define CAMERA_HAIR_APP_VBO_H

#include<glm/glm.hpp>
#include <GLES3/gl3.h>
#include<vector>

#include "globals.h"

// Structure to standardize the vertices used in the meshes
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

class VBO
{
public:
    // Reference ID of the Vertex Buffer Object
    GLuint ID;
    // Constructor that generates a Vertex Buffer Object and links it to vertices
    VBO(std::vector<Vertex>& vertices);

    // Binds the VBO
    void Bind();
    // Unbinds the VBO
    void Unbind();
    // Deletes the VBO
    void Delete();
};

#endif //CAMERA_HAIR_APP_VBO_H
