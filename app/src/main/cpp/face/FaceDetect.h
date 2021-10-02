//
// Created by jooho on 2021-09-05.
//

#ifndef CAMERA_HAIR_APP_FACEDETECT_H
#define CAMERA_HAIR_APP_FACEDETECT_H

#include <android/asset_manager.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/face.hpp"
#include "opencv2/face/facemark.hpp"
#include "opencv2/opencv.hpp"

#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/opencv.h"

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "render/Mesh.h"
#include "globals.h"
#include "Log.h"

struct FaceDetectObj {
    std::vector<cv::Point> shape;
    cv::Point topHeadPoint;
    cv::Point botHeadPoint;
    cv::Point rightHeadPoint;
    cv::Point leftHeadPoint;
    glm::vec3 topHeadCoord;
    float width;
    float height;
    float yaw;
    float pitch;
    float roll;
};

class FaceDetect
{
public:
    FaceDetect();

    void init(std::vector<std::string> file_paths);

    std::vector<FaceDetectObj> getFaceLandmarks(unsigned char* image, int width, int height, int camera_facing);

    glm::mat4 genFaceModel(GLuint camera_facing);
    Mesh genFaceMesh(std::vector<cv::Point>& shape);

    cv::CascadeClassifier face_cascade;

    cv::Mat markers;

    dlib::shape_predictor shape_predictor;
    dlib::frontal_face_detector face_detector;

    unsigned char* face_mask_image;

    std::vector<Texture> fdTextures;    // empty list
    // Vertices coordinates for face
    std::vector<GLuint> fdIndices
            {
                    0, 17, 36,
                    0, 1, 36,
                    1, 41, 36,
                    1, 31, 41,
                    1, 2, 31,
                    2, 3, 31,
                    3, 4, 31,
                    4, 48, 31,
                    4, 5, 48,
                    5, 60, 48,
                    5, 6, 60,
                    6, 59, 60,
                    6, 7, 59,
                    7, 58, 59,
                    7, 57, 58,
                    7, 8, 57,
                    8, 56, 57,
                    8, 9, 56,
                    9, 55, 56,
                    9, 10, 55,
                    10, 64, 55,
                    10, 54, 64,
                    10, 11, 54,
                    11, 12, 54,
                    11, 12, 54,
                    13, 54, 12,
                    13, 35, 54,
                    13, 14, 35,
                    14, 15, 35,
                    15, 46, 35,
                    15, 45, 46,
                    15, 16, 45,
                    16, 26, 45,
                    26, 25, 45,
                    25, 24, 45,
                    24, 44, 45,
                    24, 23, 44,
                    23, 43, 44,
                    23, 22, 43,
                    22, 42, 43,
                    22, 27, 42,
                    27, 21, 39,
                    21, 38, 39,
                    21, 20, 38,
                    20, 19, 38,
                    19, 37, 38,
                    19, 18, 37,
                    18, 36, 37,
                    18, 17, 36,
                    36, 37, 41,
                    37, 41, 40,
                    37, 40, 38,
                    38, 40, 39,
                    42, 47, 43,
                    47, 46, 43,
                    43, 46, 44,
                    44, 46, 45,
                    27, 39, 28,
                    39, 40, 28,
                    40, 28, 29,
                    41, 40, 29,
                    41, 30, 29,
                    41, 30, 31,
                    27, 42, 28,
                    28, 47, 42,
                    28, 29, 47,
                    29, 46, 47,
                    29, 30, 46,
                    30, 35, 46,
                    30, 31, 32,
                    30, 32, 33,
                    30, 33, 34,
                    30, 34, 35,
                    31, 48, 60,
                    31, 60, 49,
                    31, 49, 32,
                    32, 49, 50,
                    32, 50, 33,
                    33, 50, 51,
                    33, 51, 52,
                    33, 52, 34,
                    34, 52, 35,
                    35, 52, 53,
                    35, 53, 64,
                    35, 64, 54,
                    60, 49, 59,
                    49, 59, 67,
                    49, 67, 61,
                    49, 61, 50,
                    50, 61, 51,
                    51, 61, 62,
                    51, 62, 63,
                    51, 63, 52,
                    52, 63, 53,
                    61, 67, 62,
                    59, 67, 58,
                    67, 58, 66,
                    58, 66, 57,
                    67, 62, 66,
                    66, 65, 62,
                    62, 65, 63,
                    63, 65, 53,
                    66, 57, 65,
                    65, 57, 56,
                    65, 56, 55,
                    53, 65, 55,
                    53, 55, 64
            };
};

#endif //CAMERA_HAIR_APP_FACEDETECT_H
