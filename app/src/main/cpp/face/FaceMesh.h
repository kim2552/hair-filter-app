//
// Created by jooho on 2021-09-09.
//

#ifndef CAMERA_HAIR_APP_FACEMESH_H
#define CAMERA_HAIR_APP_FACEMESH_H

#include "eos/core/Image.hpp"
#include "eos/core/image/opencv_interop.hpp"
#include "eos/core/Landmark.hpp"
#include "eos/core/LandmarkMapper.hpp"
#include "eos/core/read_pts_landmarks.hpp"
#include "eos/core/write_obj.hpp"
#include "eos/fitting/fitting.hpp"
#include "eos/morphablemodel/Blendshape.hpp"
#include "eos/morphablemodel/MorphableModel.hpp"
#include "eos/render/opencv/draw_utils.hpp"
#include "eos/render/texture_extraction.hpp"
#include "eos/cpp17/optional.hpp"

#include "Mesh.h"
#include "globals.h"

struct FaceMeshObj{
    eos::core::Mesh mesh;
    float yaw;
    float pitch;
    float roll;
    eos::fitting::RenderingParameters rendering_parameters;
};

struct FaceProperties{
    glm::vec3 topHeadCoord;
    float faceWidth;
    float faceHeight;
};

class FaceMesh
{
public:

    eos::morphablemodel::MorphableModel morphable_model;
    eos::core::LandmarkMapper landmark_mapper;
    std::vector<eos::morphablemodel::Blendshape> blendshapes;
    eos::morphablemodel::MorphableModel morphable_model_with_expressions;
    eos::fitting::ModelContour model_contour;
    eos::fitting::ContourLandmarks ibug_contour;
    eos::morphablemodel::EdgeTopology edge_topology;

    FaceMesh();
    void init(std::vector<std::string> file_paths);

    eos::core::LandmarkCollection<Eigen::Vector2f> processLandmarks(std::vector<cv::Point2f> face);

    FaceMeshObj getFaceMeshObj(eos::core::LandmarkCollection<Eigen::Vector2f> landmarks, int imgWidth, int imgHeight);
    glm::mat4 genFaceModel(FaceMeshObj& faceMeshObj, GLuint camera_facing);
    Mesh genFaceMesh(FaceMeshObj& faceMeshObj);
    FaceProperties genProperties(FaceMeshObj& faceMeshObj, glm::mat4 model);

private:
    std::string modelfile;
    std::string mappingsfile;
    std::string contourfile;
    std::string edgetopologyfile;
    std::string blendshapesfile;

    std::vector<Texture> fmTextures;    // empty list
};

#endif //CAMERA_HAIR_APP_FACEMESH_H
