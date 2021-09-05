//
// Created by jooho on 2021-09-05.
//

#include "FaceDetect.h"

FaceDetect::FaceDetect(AAssetManager *mgr)
{
//    face_cascade.load("assets/data/lbpcascade_frontalface_improved.xml");
//    facemark = cv::face::createFacemarkKazemi();
//    facemark->loadModel("assets/data/face_landmark_model.dat");
}

//TODO::We will be using YUV image which will isolate gray channel image, so we don't have to conver to gray image
std::vector<std::vector<cv::Point2f>> FaceDetect::getFaceLandmarks(unsigned char* image, int width, int height)
{
    cv::Mat imageMat = cv::Mat(height, width, CV_8UC3, image);

    cv::resize(imageMat, imageMat, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT), 0, 0, cv::INTER_LINEAR_EXACT);
    cv::Mat gray;
    std::vector<cv::Rect> faces;
    if (imageMat.channels() > 1) {
        cvtColor(imageMat, gray, cv::COLOR_BGR2GRAY);
    }
    else {
        gray = imageMat.clone();
    }
    cv::equalizeHist(gray, gray);
//    face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

    std::vector<std::vector<cv::Point2f>> shapes;
    if (faces.size() > 0) {
#if ENABLE_DEBUG
//        if (facemark->fit(imageMat, faces, shapes))
//		{
//			for (size_t i = 0; i < faces.size(); i++)
//			{
//				cv::rectangle(imageMat, faces[i], cv::Scalar(255, 0, 0));
//			}
//			for (unsigned long i = 0; i < faces.size(); i++) {
//				for (unsigned long k = 0; k < shapes[i].size(); k++)
//					cv::circle(imageMat, shapes[i][k], 5, cv::Scalar(0, 0, 255), cv::FILLED);
//			}
//			//cv::namedWindow("Detected_shape");
//			//cv::imshow("Detected_shape", imageMat);
//			//cv::waitKey(0);
//		}
#else
//        facemark->fit(imageMat, faces, shapes);
#endif
    }
    return shapes;
}