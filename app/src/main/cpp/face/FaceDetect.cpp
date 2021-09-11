//
// Created by jooho on 2021-09-05.
//

#include <android/log.h>
#include "FaceDetect.h"

FaceDetect::FaceDetect()
{
}

void FaceDetect::init(std::vector<std::string> file_paths)
{
    face_cascade.load(file_paths[0]);
    facemark = cv::face::createFacemarkKazemi();
    facemark->loadModel(file_paths[1]);
}

std::vector<std::vector<cv::Point2f>> FaceDetect::getFaceLandmarks(unsigned char* image, int width, int height)
{
    // Image we get from camera, height < width, it's landscape
    cv::Mat imageMat = cv::Mat(height, width, CV_8UC1, image);

    // Size(width, height), RESIZED_IMAGE_HEIGHT > RESIZED_IMAGE_WIDTH, so use RESIZED_IMAGE_HEIGHT for width and RESIZED_IMAGE_WIDTH for height
    cv::resize(imageMat, imageMat, cv::Size(RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH), 0, 0, cv::INTER_LINEAR_EXACT);
    cv::Mat flippedImageMat = cv::Mat(height, width, CV_8UC1);
    cv::flip(imageMat, flippedImageMat, 0);

    // We rotate image by 90, width = RESIZED_IMAGE_WIDTH, height = RESIZED_IMAGE_HEIGHT
    cv::rotate(flippedImageMat, flippedImageMat, cv::ROTATE_90_COUNTERCLOCKWISE);
    std::vector<cv::Rect> faces;
    cv::equalizeHist(flippedImageMat, flippedImageMat);
    face_cascade.detectMultiScale(flippedImageMat, faces, 1.1, 6, 0, cv::Size(30, 30));

    std::vector<std::vector<cv::Point2f>> shapes;
    if (faces.size() > 0) {
        facemark->fit(flippedImageMat, faces, shapes);
        __android_log_print(ANDROID_LOG_INFO, "FaceSize", "width: %d, height: %d", faces[0].size().width, faces[0].size().height);
//        std::vector<cv::Point2f> shape;
//
//        shape.push_back(cv::Point2f(faces[0].x,faces[0].y));
//        shape.push_back(cv::Point2f(faces[0].x,faces[0].y+faces[0].height));
//        shape.push_back(cv::Point2f(faces[0].x+faces[0].width,faces[0].y+faces[0].height));
//        shape.push_back(cv::Point2f(faces[0].x+faces[0].width,faces[0].y));
//        shapes.push_back(shape);
    }

    return shapes;
}

glm::mat4 FaceDetect::genFaceModel(GLuint camera_facing)
{
    float sideLength = glm::tan(glm::radians(22.5f)) * 1.0f;			// calculate scaling face detect
    float faceMaskScaledLength = 2.0f - (2.0f * sideLength);

    glm::mat4 faceDetectModel = glm::mat4(1.0f);
//    faceDetectModel = glm::scale(faceDetectModel, glm::vec3(1.0/20.0, 1.0/20.0, 1.0/20.0));
    faceDetectModel = glm::scale(faceDetectModel, glm::vec3(faceMaskScaledLength, faceMaskScaledLength, faceMaskScaledLength));
//    faceDetectModel = glm::rotate(faceDetectModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Flip the image
    faceDetectModel = glm::translate(faceDetectModel, glm::vec3(-0.375, -0.5, -1.25f));
//    faceDetectModel = glm::translate(faceDetectModel, glm::vec3(0.375f, -0.5f, -0.875f));
    if(camera_facing == 0){
//        faceDetectModel = glm::rotate(faceDetectModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Flip the image
    }
//    if(m_cameraFacing == 0){
//        imgModel = glm::rotate(imgModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Flip the image
//        imgModel = glm::rotate(imgModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Flip the image
//    }
//    imgModel = glm::rotate(imgModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image

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

