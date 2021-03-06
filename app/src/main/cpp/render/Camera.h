//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_CAMERA_H
#define CAMERA_HAIR_APP_CAMERA_H

#include <GLES3/gl3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"render/ShaderClass.h"

class Camera
{
public:
    // Stores the main vectors of the camera
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    // Stores the width and height of the window
    int width;
    int height;

    // Adjust the speed of the camera and it's sensitivity when looking around
    float speed = 0.1f;
    float sensitivity = 100.0f;

    // Camera constructor to set up initial values
    Camera(int width, int height, glm::vec3 position);

    // Updates the camera matrix to the Vertex Shader
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    // Exports the camera matrix to a shader
    void Matrix(Shader& shader, const char* uniform);
//    // Handles camera inputs          TODO::Phone Inputs?
//    void Inputs(GLFWwindow* window);
};

#endif //CAMERA_HAIR_APP_CAMERA_H
