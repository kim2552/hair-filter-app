//
// Created by jooho on 2021-08-26.
//

#ifndef CAMERA_HAIR_APP_SHADERCLASS_H
#define CAMERA_HAIR_APP_SHADERCLASS_H

#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <GLES3/gl3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#include "globals.h"
#include "GLShaders.h"

std::string get_file_contents(const char* filename);

class Shader
{
public:
    // Reference ID of the Shader Program
    GLuint ID;
    // Constructor that build the Shader Program from 2 different shaders
    Shader(const char* vertexFile, const char* fragmentFile);

    // Activates the Shader Program
    void Activate();
    // Deletes the Shader Program
    void Delete();

    // Utility Uniform Functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    void compileErrors(unsigned int shader, const char* type);
};

#endif // !SHADER_CLASS_H


#endif //CAMERA_HAIR_APP_SHADERCLASS_H
