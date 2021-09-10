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
    facemark = cv::face::FacemarkKazemi::create();
    facemark->loadModel(file_paths[1]);
}

//TODO::We will be using YUV image which will isolate gray channel image, so we don't have to convert to gray image
std::vector<std::vector<cv::Point2f>> FaceDetect::getFaceLandmarks(unsigned char* image, int width, int height)
{
    cv::Mat imageMat = cv::Mat(height, width, CV_8UC1, image);

    cv::resize(imageMat, imageMat, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT), 0, 0, cv::INTER_LINEAR_EXACT);
    cv::rotate(imageMat, imageMat, cv::ROTATE_90_COUNTERCLOCKWISE);
    std::vector<cv::Rect> faces;
    cv::equalizeHist(imageMat, imageMat);
    face_cascade.detectMultiScale(imageMat, faces, 1.1, 3, 0, cv::Size(30, 30));

    std::vector<std::vector<cv::Point2f>> shapes;
    if (faces.size() > 0) {
        facemark->fit(imageMat, faces, shapes);
        __android_log_print(ANDROID_LOG_INFO, "FaceDetect.cpp", "%s", "FACE DETECTED.");
    }
    return shapes;
}

glm::mat4 FaceDetect::genFaceModel(GLuint camera_facing)
{
    float sideLength = glm::tan(glm::radians(22.5f)) * 1.0f;			// calculate scaling face detect
    float faceMaskScaledLength = 2.0f - (2.0f * sideLength);

    glm::mat4 faceDetectModel = glm::mat4(1.0f);
//    faceDetectModel = glm::scale(faceDetectModel, glm::vec3(1.0/20.0, 1.0/20.0, 1.0/20.0));
    faceDetectModel = glm::rotate(faceDetectModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Flip the image
    faceDetectModel = glm::translate(faceDetectModel, glm::vec3(-0.5, -0.5, 0.875f));
    faceDetectModel = glm::scale(faceDetectModel, glm::vec3(faceMaskScaledLength, faceMaskScaledLength, faceMaskScaledLength));
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