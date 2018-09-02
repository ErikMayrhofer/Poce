#include <utility>

//
// Created by obyoxar on 01.09.18.
//
#include "monitoring.h"
#include "FaceDetector.h"
#include <opencv2/highgui.hpp>

FaceDetector::FaceDetector(std::string modelPath) : pose_model(){
    deserialize(modelPath) >> pose_model;
}

FaceDetector::~FaceDetector() {
    std::cout << "HI" << std::endl;
};

std::vector<FaceDetector::Rect> FaceDetector::detectOutlines(cv_image<rgb_pixel> image) {
    std::vector<rectangle> rects = detector(image);
    return rects;
}

std::vector<FaceDetector::Point> FaceDetector::detectNoses(cv::Mat image, cv::Size viewSize) {
    cv::Mat scaled;
    cv::resize(image, scaled, viewSize);
    cv_image<rgb_pixel> cimg(scaled);
    auto dets = this->detectOutlines(cimg);
    std::vector<FaceDetector::Point> noses(dets.size());
    auto noseIterator = noses.begin();
    for (const auto &face : dets){
        full_object_detection detection = pose_model(cimg, face);
        dlib::point noseTip = detection.part(33);
        *(noseIterator++) = FaceDetector::Point(
                noseTip.x() / (float)viewSize.width,
                noseTip.y() / (float)viewSize.height
                );
    }
    return noses;
}
/*
std::vector<point> FaceDetector::detectFeatures(cv::Mat image, rectangle) {
    full_object_detection detection = pose_model(cimg, face);
}
*/

