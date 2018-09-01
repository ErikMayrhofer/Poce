//
// Created by obyoxar on 01.09.18.
//

#ifndef POCEEXP_FACEDETECTOR_H
#define POCEEXP_FACEDETECTOR_H

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/geometry/vector.h>

using namespace dlib;

class FaceDetector {
public:
    typedef rectangle Rect;
    typedef cv::Point2d Point;
    explicit FaceDetector(std::string modelPath);
    ~ FaceDetector();

    std::vector<FaceDetector::Rect> detectOutlines(cv_image<rgb_pixel> image);
    std::vector<FaceDetector::Point> detectNoses(cv::Mat image, cv::Size viewSize);
    //std::vector<point> detectFeatures(cv::Mat image, rectangle);


private:
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor pose_model;
};


#endif //POCEEXP_FACEDETECTOR_H
