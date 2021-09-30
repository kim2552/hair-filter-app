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
    face_cascade.load(file_paths[1]);
    dlib::deserialize(file_paths[2]) >> shape_predictor;
}

std::vector<FaceDetectObj> FaceDetect::getFaceLandmarks(unsigned char* image, int width, int height)
{
    markers.create(cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT), CV_8UC1);
    markers.setTo(cv::Scalar(0.0));

    markers.setTo(cv::Scalar(0.0));
    std::vector<FaceDetectObj> faceDetectObjs;

    // Image we get from camera, height < width, it's landscape
    cv::Mat imageMat = cv::Mat(height, width, CV_8UC1, image);
    // Size(width, height), RESIZED_IMAGE_HEIGHT > RESIZED_IMAGE_WIDTH, so use RESIZED_IMAGE_HEIGHT for width and RESIZED_IMAGE_WIDTH for height
    cv::resize(imageMat, imageMat, cv::Size(RESIZED_IMAGE_HEIGHT, RESIZED_IMAGE_WIDTH), 0, 0, cv::INTER_AREA);
    // We rotate image by 90, width = RESIZED_IMAGE_WIDTH, height = RESIZED_IMAGE_HEIGHT
    cv::rotate(imageMat, imageMat, cv::ROTATE_90_COUNTERCLOCKWISE);
    cv::equalizeHist(imageMat, imageMat);

    std::vector<cv::Rect> faces;
    // Minimum face size is 1/5th of screen height
    // Maximum face size is 2/3rds of screen height
    face_cascade.detectMultiScale(imageMat, faces, 1.1, 3, 0,
                                  cv::Size(RESIZED_IMAGE_HEIGHT / 5, RESIZED_IMAGE_HEIGHT / 5),
                                  cv::Size(RESIZED_IMAGE_HEIGHT * 2 / 3, RESIZED_IMAGE_HEIGHT * 2 / 3));

    for (unsigned long j = 0; j < faces.size(); ++j)
    {
        FaceDetectObj obj;

        //Face landmarks using dlib
        dlib::rectangle rect = dlib::rectangle((long)faces[j].tl().x, (long)faces[j].tl().y, (long)faces[j].br().x - 1, (long)faces[j].br().y - 1);
        dlib::array2d<dlib::rgb_pixel> img_dlib;
        dlib::assign_image(img_dlib, dlib::cv_image<unsigned char>(imageMat));
        dlib::full_object_detection shape = shape_predictor(img_dlib, rect);

        //Face Pose variables
        cv::Mat rotation_vec;                           //3 x 1
        cv::Mat rotation_mat;                           //3 x 3 R
        cv::Mat translation_vec;                        //3 x 1 T
        cv::Mat pose_mat = cv::Mat(3, 4, CV_64FC1);     //3 x 4 R | T
        cv::Mat euler_angle = cv::Mat(3, 1, CV_64FC1);

        // Face landmark points
        std::vector<cv::Point> pts;
        std::vector<cv::Point2d> pose_image_points;

        // Key face landmarks
        cv::Point2d left_brow_left_corner;
        cv::Point2d left_brow_right_corner;
        cv::Point2d right_brow_left_corner;
        cv::Point2d right_brow_right_corner;
        cv::Point2d left_eye_left_corner;
        cv::Point2d left_eye_right_corner;
        cv::Point2d right_eye_left_corner;
        cv::Point2d right_eye_right_corner;
        cv::Point2d nose_left_corner;
        cv::Point2d nose_right_corner;
        cv::Point2d mouth_left_corner;
        cv::Point2d mouth_right_corner;
        cv::Point2d mouth_central_bottom_corner;
        cv::Point2d chin_corner;

        for (unsigned long k = 0; k < shape.num_parts(); k++) {
            cv::Point p;
            p.x = shape.part(k).x();
            p.y = shape.part(k).y();
            pts.push_back(p);
            if (k == 8) {
                std::cout << "bot: " << p.x << "," << p.y << std::endl;
                cv::Point p_transformed;
                p_transformed.x = p.x;
                p_transformed.y = RESIZED_IMAGE_HEIGHT - p.y;
                obj.botHeadPoint = p_transformed;
            }
            if (k == 71) {
                std::cout << "top: " << p.x << "," << p.y << std::endl;
                cv::Point p_transformed;
                p_transformed.x = p.x;
                p_transformed.y = RESIZED_IMAGE_HEIGHT - p.y;
                obj.topHeadPoint = p_transformed;
            }
            if (k == 0) {
                std::cout << "right: " << p.x << "," << p.y << std::endl;
                cv::Point p_transformed;
                p_transformed.x = p.x;
                p_transformed.y = RESIZED_IMAGE_HEIGHT - p.y;
                obj.rightHeadPoint = p_transformed;
            }
            if (k == 16) {
                std::cout << "left: " << p.x << "," << p.y << std::endl;
                cv::Point p_transformed;
                p_transformed.x = p.x;
                p_transformed.y = RESIZED_IMAGE_HEIGHT - p.y;
                obj.leftHeadPoint = p_transformed;
            }
//            if (k == 5) {
//                cv::Point p_transformed;
//                p_transformed.x = p.x;
//                p_transformed.y = RESIZED_IMAGE_HEIGHT;
//                pts.push_back(p_transformed);
//            }
//            if (k == 11) {
//                cv::Point p_transformed;
//                p_transformed.x = p.x;
//                p_transformed.y = RESIZED_IMAGE_HEIGHT;
//                pts.push_back(p_transformed);
//            }
            if (k == 57) {
                mouth_central_bottom_corner = p;
            }
            if (k == 54) {
                mouth_right_corner = p;
            }
            if (k == 48) {
                mouth_left_corner = p;
            }
            if (k == 45) {
                right_eye_right_corner = p;
            }
            if (k == 42) {
                right_eye_left_corner = p;
            }
            if (k == 39) {
                left_eye_right_corner = p;
            }
            if (k == 36) {
                left_eye_left_corner = p;
            }
            if (k == 35) {
                nose_right_corner = p;
            }
            if (k == 31) {
                nose_left_corner = p;
            }
            if (k == 26) {
                right_brow_right_corner = p;
            }
            if (k == 22) {
                right_brow_left_corner = p;
            }
            if (k == 21) {
                left_brow_right_corner = p;
            }
            if (k == 17) {
                left_brow_left_corner = p;
            }
            if (k == 8) {
                chin_corner = p;
            }
        }

        // Create contour and draw on mask
        std::vector<cv::Point> contour_hull;
        cv::convexHull(pts, contour_hull);
        std::vector<std::vector<cv::Point>> hulls;
        hulls.push_back(contour_hull);
        cv::drawContours(markers, hulls, 0, cv::Scalar(255),-1);

        // Pose image points
        pose_image_points.push_back(left_brow_left_corner);
        pose_image_points.push_back(left_brow_right_corner);
        pose_image_points.push_back(right_brow_left_corner);
        pose_image_points.push_back(right_brow_right_corner);
        pose_image_points.push_back(left_eye_left_corner);
        pose_image_points.push_back(left_eye_right_corner);
        pose_image_points.push_back(right_eye_left_corner);
        pose_image_points.push_back(right_eye_right_corner);
        pose_image_points.push_back(nose_left_corner);
        pose_image_points.push_back(nose_right_corner);
        pose_image_points.push_back(mouth_left_corner);
        pose_image_points.push_back(mouth_right_corner);
        pose_image_points.push_back(mouth_central_bottom_corner);
        pose_image_points.push_back(chin_corner);

        //fill in 3D ref points(world coordinates), model referenced from http://aifi.isr.uc.pt/Downloads/OpenGL/glAnthropometric3DModel.cpp
        std::vector<cv::Point3d> object_pts;
        object_pts.push_back(cv::Point3d(6.825897, 6.760612, 4.402142));     //#33 left brow left corner
        object_pts.push_back(cv::Point3d(1.330353, 7.122144, 6.903745));     //#29 left brow right corner
        object_pts.push_back(cv::Point3d(-1.330353, 7.122144, 6.903745));    //#34 right brow left corner
        object_pts.push_back(cv::Point3d(-6.825897, 6.760612, 4.402142));    //#38 right brow right corner
        object_pts.push_back(cv::Point3d(5.311432, 5.485328, 3.987654));     //#13 left eye left corner
        object_pts.push_back(cv::Point3d(1.789930, 5.393625, 4.413414));     //#17 left eye right corner
        object_pts.push_back(cv::Point3d(-1.789930, 5.393625, 4.413414));    //#25 right eye left corner
        object_pts.push_back(cv::Point3d(-5.311432, 5.485328, 3.987654));    //#21 right eye right corner
        object_pts.push_back(cv::Point3d(2.005628, 1.409845, 6.165652));     //#55 nose left corner
        object_pts.push_back(cv::Point3d(-2.005628, 1.409845, 6.165652));    //#49 nose right corner
        object_pts.push_back(cv::Point3d(2.774015, -2.080775, 5.048531));    //#43 mouth left corner
        object_pts.push_back(cv::Point3d(-2.774015, -2.080775, 5.048531));   //#39 mouth right corner
        object_pts.push_back(cv::Point3d(0.000000, -3.116408, 6.097667));    //#45 mouth central bottom corner
        object_pts.push_back(cv::Point3d(0.000000, -7.415691, 4.070434));    //#6 chin corner

        // Camera internals
        cv::Mat cam_matrix = cv::Mat(3, 3, CV_64FC1, K);
        cv::Mat dist_coeffs = cv::Mat(5, 1, CV_64FC1, D);

        // Solve for pose
        cv::solvePnP(object_pts, pose_image_points, cam_matrix, dist_coeffs, rotation_vec, translation_vec);

        //temp buf for decomposeProjectionMatrix()
        cv::Mat out_intrinsics = cv::Mat(3, 3, CV_64FC1);
        cv::Mat out_rotation = cv::Mat(3, 3, CV_64FC1);
        cv::Mat out_translation = cv::Mat(3, 1, CV_64FC1);

        //calc euler angle
        cv::Rodrigues(rotation_vec, rotation_mat);
        cv::hconcat(rotation_mat, translation_vec, pose_mat);
        cv::decomposeProjectionMatrix(pose_mat, out_intrinsics, out_rotation, out_translation, cv::noArray(), cv::noArray(), cv::noArray(), euler_angle);

        obj.pitch = euler_angle.at<double>(0);// *180.0f / 3.1415926535897932384;
        obj.yaw = euler_angle.at<double>(1);// *180.0f / 3.1415926535897932384;
        obj.roll = euler_angle.at<double>(2);// *180.0f / 3.1415926535897932384;

        obj.shape = pts;
        faceDetectObjs.push_back(obj);
    }
    cv::resize(markers, markers, cv::Size(height, width), 0, 0, cv::INTER_NEAREST);
    cv::rotate(markers, markers, cv::ROTATE_90_CLOCKWISE);
    face_mask_image = markers.data;

    return faceDetectObjs;
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

Mesh FaceDetect::genFaceMesh(std::vector<cv::Point>& shape)
{
    Vertex faceVertex[68];      // Extra two points are for the neck
    for (size_t j = 0; j < 68; j++){					// Loop through landmarks in face
        faceVertex[j]= Vertex{ glm::vec3((float)shape[j].x /(float)RESIZED_IMAGE_HEIGHT, (float)shape[j].y * IMAGE_ASPECT_RATIO /(float)RESIZED_IMAGE_WIDTH,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) };
    }

    // Store mesh data in vectors for the mesh
    std::vector <Vertex> fdVerts(faceVertex, faceVertex + sizeof(faceVertex) / sizeof(Vertex));

    return Mesh(fdVerts, fdIndices, fdTextures);
}