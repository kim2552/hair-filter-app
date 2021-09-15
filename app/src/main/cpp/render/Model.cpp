//
// Created by jooho on 2021-09-14.
//

#include "Model.h"

Model::Model(std::vector<Mesh> m, int winWidth, int winHeight)
{
    meshes = m;
    windowWidth = winWidth;
    windowHeight = winHeight;
}

void Model::Draw(Shader& shader, Camera& camera)
{
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader, camera, modelMat);
    }
}

void Model::UpdateModel(glm::mat4 model)
{
    modelMat = model;			// Update the modelMat

    glm::vec4 vertexPoint = glm::vec4(meshes[0].vertices[0].position, 1.0f);
    fixedVertex = modelMat * vertexPoint;

    glm::vec4 vmin = modelMat * glm::vec4(originalBb.min,1.0f);
    bb.min = glm::vec3(vmin.x, vmin.y, vmin.z);
    glm::vec4 vmax = modelMat * glm::vec4(originalBb.max, 1.0f);
    bb.max = glm::vec3(vmax.x, vmax.y, vmax.z);

    modelWidth = abs(bb.max.x - bb.min.x);
    modelHeight = abs(bb.max.y - bb.min.y);

    position = modelMat * glm::vec4(originalPosition,1.0f);
}