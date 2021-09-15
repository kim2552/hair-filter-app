//
// Created by jooho on 2021-09-14.
//

#ifndef CAMERA_HAIR_APP_MODEL_H
#define CAMERA_HAIR_APP_MODEL_H

#include "Mesh.h"
#include "face/FaceMesh.h"

struct BoundingBox
{
    glm::vec3 min;
    glm::vec3 max;
};

class Model
{
public:
    Model(std::vector<Mesh> meshes, int winWidth=0, int winHeight=0);

    // Draw the model
    void Draw(Shader& shader, Camera& camera);

    // Updates the model matrix
    void UpdateModel(glm::mat4 model);

    // BoundingBox of the object
    BoundingBox originalBb;
    BoundingBox bb;

    // Corresponding Face Parameters
    glm::vec3 topHeadCoord;
    float faceWidth;
    float faceHeight;

    // All the meshes and transformations
    std::vector<Mesh> meshes;
    glm::mat4 modelMat = glm::mat4(1.0f);

    // Prevents textures from being loaded twice
    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    // Model world coordinate information
    glm::vec3 fixedVertex;						// This fixed vertex point, is a consistent vertex point on the mesh
    glm::vec3 originalPosition;
    glm::vec3 position;
    float modelWidth;
    float modelHeight;
    float roll;
    float pitch;
    float yaw;

    // Stores the width and height of the window
    int windowWidth;
    int windowHeight;
};

#endif //CAMERA_HAIR_APP_MODEL_H
