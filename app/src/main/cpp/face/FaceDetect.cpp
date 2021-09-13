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
    facemark = cv::face::createFacemarkLBF();
    facemark->loadModel(file_paths[1]);
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

//    cv::Mat sharpenedImage;
//    cv::GaussianBlur(imageMat, sharpenedImage, cv::Size(0, 0), 3);
//    cv::addWeighted(imageMat, 1.5, sharpenedImage, -0.5, 0, sharpenedImage);
    imageMat.convertTo(imageMat, -1, 1, 100); //increase the brightness by 100
//    cv::addWeighted(imageMat, 1.5, imageMat, -0.5, 0, imageMat);
    face_cascade.detectMultiScale(imageMat, faces, 1.05, 3, 0, cv::Size(50, 50));

    std::vector<std::vector<cv::Point2f>> shapes;
    if (faces.size() > 0) {
        if(facemark->fit(imageMat, faces, shapes)){
            for (size_t i = 0; i < faces.size(); i++)
            {
                cv::rectangle(imageMat, faces[i], cv::Scalar(255, 0, 0));
            }
            for (unsigned long i = 0; i < faces.size(); i++) {
                for (unsigned long k = 0; k < shapes[i].size(); k++)
                    cv::circle(imageMat, shapes[i][k], 2, cv::Scalar(0, 0, 255), cv::FILLED);
            }
        }
        __android_log_print(ANDROID_LOG_INFO, "FaceSize", "width: %d, height: %d", faces[0].size().width, faces[0].size().height);
    }
    if(count == 50) {
        std::string imageString = "";
        for (int i = 0; i < RESIZED_IMAGE_WIDTH * RESIZED_IMAGE_HEIGHT; i++) {
            if (i % 150 == 0) {
                LOGE("%s", imageString.c_str());
                imageString = "";
            }
            imageString += std::to_string(imageMat.data[i]) + " ";
        }
        LOGE("%s", imageString.c_str());
    }
    count++;
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

