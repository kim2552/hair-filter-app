//
// Created by jooho on 2021-09-09.
//

#include "FaceMesh.h"

FaceMesh::FaceMesh()
{
}

void FaceMesh::init(std::vector<std::string> file_paths)
{
    modelfile = file_paths[2];
    mappingsfile = file_paths[3];
    contourfile = file_paths[4];
    edgetopologyfile = file_paths[5];
    blendshapesfile = file_paths[6];

    morphable_model = eos::morphablemodel::load_model(modelfile);

    landmark_mapper = eos::core::LandmarkMapper(mappingsfile);

    blendshapes = eos::morphablemodel::load_blendshapes(blendshapesfile);

    morphable_model_with_expressions = eos::morphablemodel::MorphableModel(
            morphable_model.get_shape_model(), blendshapes, morphable_model.get_color_model(), eos::cpp17::nullopt,
            morphable_model.get_texture_coordinates());

    // These two are used to fit the front-facing contour to the ibug contour landmarks:
    model_contour = eos::fitting::ModelContour::load(contourfile);
    ibug_contour = eos::fitting::ContourLandmarks::load(mappingsfile);

    // The edge topology is used to speed up computation of the occluding face contour fitting:
    //edge_topology = eos::morphablemodel::load_edge_topology(edgetopologyfile);
}


/* Process facial landmarks and store them in a LandmarkCollection object*/
eos::core::LandmarkCollection<Eigen::Vector2f> FaceMesh::processLandmarks(std::vector<cv::Point2f> face)
{
    eos::core::LandmarkCollection<Eigen::Vector2f> landmarks;
    landmarks.reserve(68);

    int ibugId = 1;
    for (size_t i = 0; i < face.size(); i++) {
        eos::core::Landmark<Eigen::Vector2f> landmark;

        landmark.name = std::to_string(ibugId);
        landmark.coordinates[0] = face[i].x;
        landmark.coordinates[1] = face[i].y;	//TODO::Might have to shift by 1.0f
        landmarks.emplace_back(landmark);
        ++ibugId;
    }

    return landmarks;
}

FaceMeshObj FaceMesh::getFaceMeshObj(eos::core::LandmarkCollection<Eigen::Vector2f> landmarks, int imgWidth, int imgHeight)
{
    FaceMeshObj meshObj;

    // Fit the model, get back a mesh and the pose:
    std::tie(meshObj.mesh, meshObj.rendering_parameters) = eos::fitting::fit_shape_and_pose(
            morphable_model_with_expressions, landmarks, landmark_mapper, imgWidth, imgHeight, edge_topology,
            ibug_contour, model_contour, 1, eos::cpp17::nullopt, 30.0f);

    // The 3D head pose can be recovered as follows:
    meshObj.yaw = glm::degrees(glm::yaw(meshObj.rendering_parameters.get_rotation()));
    meshObj.pitch = glm::degrees(glm::pitch(meshObj.rendering_parameters.get_rotation()));
    meshObj.roll = glm::degrees(glm::roll(meshObj.rendering_parameters.get_rotation()));

    return meshObj;
}

Mesh FaceMesh::genFaceMesh(FaceMeshObj& faceMeshObj)
{
    // Initialize all of the vertices
    std::vector<Vertex> facemeshVerts;
    for (size_t i = 0; i < faceMeshObj.mesh.vertices.size(); i++)
    {
        // Get original location of the vertex
        const auto p = glm::project(
                { faceMeshObj.mesh.vertices[i][0], faceMeshObj.mesh.vertices[i][1], faceMeshObj.mesh.vertices[i][2] },
                faceMeshObj.rendering_parameters.get_modelview(), faceMeshObj.rendering_parameters.get_projection(), glm::vec4(0, RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH, -RESIZED_IMAGE_HEIGHT));

        // Add the vertex after modifying based on aspect ratio
        facemeshVerts.push_back(Vertex{ glm::vec3(p.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH, p.y / (float)RESIZED_IMAGE_HEIGHT, -p.z / (float)RESIZED_IMAGE_HEIGHT), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(faceMeshObj.mesh.texcoords[i](0), faceMeshObj.mesh.texcoords[i](1)) });
    }

    // Initialize all of the indices
    std::vector<GLuint> facemeshInds;
    for (size_t i = 0; i < faceMeshObj.mesh.tvi.size(); i++)
    {
        for (size_t j = 0; j < faceMeshObj.mesh.tvi[i].size(); j++)
        {
            facemeshInds.push_back(faceMeshObj.mesh.tvi[i][j]);
        }
    }

    Mesh faceMesh = Mesh(facemeshVerts, facemeshInds, fmTextures);
    return faceMesh;
}

glm::mat4 FaceMesh::genFaceModel(FaceMeshObj& faceMeshObj, GLuint camera_facing)
{
    // calculate scaling face object
    float sideLength = glm::tan(glm::radians(22.5f)) * 0.95f;
    float faceMeshScaledLength = 2.0f - (2.0f * sideLength);

    glm::mat4 faceMeshModel = glm::mat4(1.0f);
    faceMeshModel = glm::scale(faceMeshModel, glm::vec3(faceMeshScaledLength, faceMeshScaledLength, faceMeshScaledLength));
    faceMeshModel = glm::translate(faceMeshModel, glm::vec3(0.375f, -0.5f, -0.875f));
    faceMeshModel = glm::rotate(faceMeshModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    return faceMeshModel;
}

FaceProperties FaceMesh::genProperties(FaceMeshObj& faceMeshObj, glm::mat4 model)
{
    FaceProperties properties;

    const auto th = glm::project(
            { faceMeshObj.mesh.vertices[FACE_MESH_TOPHEAD_INDEX][0], faceMeshObj.mesh.vertices[FACE_MESH_TOPHEAD_INDEX][1], faceMeshObj.mesh.vertices[FACE_MESH_TOPHEAD_INDEX][2] },
            faceMeshObj.rendering_parameters.get_modelview(), faceMeshObj.rendering_parameters.get_projection(), glm::vec4(0, RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH, -RESIZED_IMAGE_HEIGHT));
    const auto bh = glm::project(
            { faceMeshObj.mesh.vertices[FACE_MESH_BOTHEAD_INDEX][0], faceMeshObj.mesh.vertices[FACE_MESH_BOTHEAD_INDEX][1], faceMeshObj.mesh.vertices[FACE_MESH_BOTHEAD_INDEX][2] },
            faceMeshObj.rendering_parameters.get_modelview(), faceMeshObj.rendering_parameters.get_projection(), glm::vec4(0, RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH, -RESIZED_IMAGE_HEIGHT));
    const auto rc = glm::project(
            { faceMeshObj.mesh.vertices[FACE_MESH_RIGHTCONTOUR_INDEX][0], faceMeshObj.mesh.vertices[FACE_MESH_RIGHTCONTOUR_INDEX][1], faceMeshObj.mesh.vertices[FACE_MESH_RIGHTCONTOUR_INDEX][2] },
            faceMeshObj.rendering_parameters.get_modelview(), faceMeshObj.rendering_parameters.get_projection(), glm::vec4(0, RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH, -RESIZED_IMAGE_HEIGHT));
    const auto lc = glm::project(
            { faceMeshObj.mesh.vertices[FACE_MESH_LEFTCONTOUR_INDEX][0], faceMeshObj.mesh.vertices[FACE_MESH_LEFTCONTOUR_INDEX][1], faceMeshObj.mesh.vertices[FACE_MESH_LEFTCONTOUR_INDEX][2] },
            faceMeshObj.rendering_parameters.get_modelview(), faceMeshObj.rendering_parameters.get_projection(), glm::vec4(0, RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH, -RESIZED_IMAGE_HEIGHT));

    glm::vec4 vth(glm::vec3(th.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH, th.y / (float)RESIZED_IMAGE_HEIGHT, -th.z / (float)RESIZED_IMAGE_HEIGHT), 1.0f);
    glm::vec4 vbh(glm::vec3(bh.x* IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH, bh.y / (float)RESIZED_IMAGE_HEIGHT, -bh.z / (float)RESIZED_IMAGE_HEIGHT), 1.0f);
    glm::vec4 vrc(glm::vec3(rc.x* IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH, rc.y / (float)RESIZED_IMAGE_HEIGHT, -rc.z / (float)RESIZED_IMAGE_HEIGHT), 1.0f);
    glm::vec4 vlc(glm::vec3(lc.x* IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH, lc.y / (float)RESIZED_IMAGE_HEIGHT, -lc.z / (float)RESIZED_IMAGE_HEIGHT), 1.0f);

    vth = model * vth;
    vbh = model * vbh;
    vrc = model * vrc;
    vlc = model * vlc;
    properties.topHeadCoord = vth;
    properties.faceWidth = glm::length(vrc - vlc);
    properties.faceHeight = glm::length(vth - vbh);

    return properties;
}