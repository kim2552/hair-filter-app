//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_MESH_H
#define CAMERA_HAIR_APP_MESH_H

#include<vector>
#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

class Mesh
{
public:
    std::vector <Vertex> vertices;
    std::vector <GLuint> indices;
    std::vector <Texture> textures;
    // Store VAO in public so it can be used in the Draw function
    VAO m_VAO;

    // Initializes the mesh
    Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

    // Draws the mesh
    void Draw
            (
                    Shader& shader,
                    Camera& camera,
                    glm::mat4 matrix = glm::mat4(1.0f),
                    GLenum mode=GL_TRIANGLES,
                    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
            );

    void Delete();
};

#endif //CAMERA_HAIR_APP_MESH_H
