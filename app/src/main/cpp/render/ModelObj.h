//
// Created by jooho on 2021-09-14.
//

#ifndef CAMERA_HAIR_APP_MODELOBJ_H
#define CAMERA_HAIR_APP_MODELOBJ_H

#include "include/tinyobj/tiny_obj_loader.h"
#include "Mesh.h"
#include "Model.h"

class ModelObj
{
public:
    // Loads in a model from a file and stores tha information in 'data', 'JSON', and 'file'
    ModelObj(std::string file, std::vector<Texture> textures);
    Model* model;

private:

    std::string filename;

    // Prevents textures from being loaded twice
    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;
};

#endif //CAMERA_HAIR_APP_MODELOBJ_H
