//
// Created by jooho on 2021-09-05.
//

#include <android/log.h>
#include "FaceDetect.h"

//Intrisics can be calculated using opencv sample code under opencv/sources/samples/cpp/tutorial_code/calib3d
//Normally, you can also apprximate fx and fy by image width, cx by half image width, cy by half image height instead
double K[9] = { 6.5308391993466671e+002, 0.0, 3.1950000000000000e+002, 0.0, 6.5308391993466671e+002, 2.3950000000000000e+002, 0.0, 0.0, 1.0 };
double D[5] = { 7.0834633684407095e-002, 6.9140193737175351e-002, 0.0, 0.0, -1.3073460323689292e+000 };

FaceDetect::FaceDetect()
{
}

void FaceDetect::init(std::vector<std::string> file_paths)
{
    // Load face cascade and face landmarks files
    face_cascade.load(file_paths[0]);
    dlib::deserialize(file_paths[1]) >> shape_predictor;

    markers.create(cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT), CV_8UC1);
    markers.setTo(cv::Scalar(0.0));
}

std::vector<std::vector<cv::Point2f>> FaceDetect::getFaceLandmarks(unsigned char* image, int width, int height)
{
    static int count = 0;
    uint8_t debug_image[RESIZED_IMAGE_WIDTH*RESIZED_IMAGE_HEIGHT];

    // Image we get from camera, height < width, it's landscape
    cv::Mat imageMat = cv::Mat(height, width, CV_8UC1, image);
    // Size(width, height), RESIZED_IMAGE_HEIGHT > RESIZED_IMAGE_WIDTH, so use RESIZED_IMAGE_HEIGHT for width and RESIZED_IMAGE_WIDTH for height
    cv::resize(imageMat, imageMat, cv::Size(RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH), 0, 0, cv::INTER_AREA);
    // We rotate image by 90, width = RESIZED_IMAGE_WIDTH, height = RESIZED_IMAGE_HEIGHT
    cv::rotate(imageMat, imageMat, cv::ROTATE_90_COUNTERCLOCKWISE);

    std::vector<cv::Rect> faces;
    cv::equalizeHist(imageMat, imageMat);

    cv::Mat sharpenedImage;
    cv::GaussianBlur(imageMat, sharpenedImage, cv::Size(0, 0), 3);
    cv::addWeighted(imageMat, 1.5, sharpenedImage, -0.5, 0, sharpenedImage);

    // Minimum face size is 1/5th of screen height
    // Maximum face size is 2/3rds of screen height
    face_cascade.detectMultiScale(sharpenedImage, faces, 1.1, 3, 0,
                                  cv::Size(RESIZED_IMAGE_HEIGHT / 5, RESIZED_IMAGE_HEIGHT / 5),
                                  cv::Size(RESIZED_IMAGE_HEIGHT * 2 / 3, RESIZED_IMAGE_HEIGHT * 2 / 3));

    std::vector<std::vector<cv::Point2f>> shapes;

    return shapes;
}

glm::mat4 FaceDetect::genFaceModel(GLuint camera_facing)
{
    float sideLength = glm::tan(glm::radians(22.5f)) * 1.0f;			// calculate scaling face detect
    float faceMaskScaledLength = 2.0f - (2.0f * sideLength);

    glm::mat4 faceDetectModel = glm::mat4(1.0f);
    faceDetectModel = glm::scale(faceDetectModel, glm::vec3(faceMaskScaledLength, faceMaskScaledLength, faceMaskScaledLength));
    if(camera_facing == 0){
        faceDetectModel = glm::rotate(faceDetectModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Flip the image
    }
    faceDetectModel = glm::translate(faceDetectModel, glm::vec3(-0.375, -0.5, 0.875f));

    return faceDetectModel;
}

Mesh FaceDetect::genFaceMesh(std::vector<cv::Point2f>& face)
{
    Vertex faceVertex[70];      // Extra two points are for the neck
    for (size_t j = 0; j < face.size(); j++){					// Loop through landmarks in face
        faceVertex[j]= Vertex{ glm::vec3((float)face[j].x /(float)RESIZED_IMAGE_HEIGHT, (float)face[j].y * IMAGE_ASPECT_RATIO /(float)RESIZED_IMAGE_WIDTH,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) };
    }
    faceVertex[68] = Vertex{ glm::vec3((float)face[5].x / (float)RESIZED_IMAGE_HEIGHT, (float)(face[5].y+500) * IMAGE_ASPECT_RATIO /(float)RESIZED_IMAGE_WIDTH,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) };

    faceVertex[69] = Vertex{ glm::vec3((float)face[11].x / (float)RESIZED_IMAGE_HEIGHT, (float)(face[11].y + 500) * IMAGE_ASPECT_RATIO /(float)RESIZED_IMAGE_WIDTH,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) };

    // Store mesh data in vectors for the mesh
    std::vector <Vertex> fdVerts(faceVertex, faceVertex + sizeof(faceVertex) / sizeof(Vertex));

    return Mesh(fdVerts, fdIndices, fdTextures);
}