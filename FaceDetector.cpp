#include <utility>

//
// Created by obyoxar on 01.09.18.
//
#include "monitoring.h"
#include "FaceDetector.h"
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>
#include "profiler.h"
#include <dlib/pixel.h>
#include <dlib/opencv/cv_image.h>

FaceDetector::FaceDetector(std::string modelPath) : pose_model(){
    deserialize(modelPath) >> pose_model;


    DLIB_USE_BLAS
}

FaceDetector::~FaceDetector() = default;;

std::vector<FaceDetector::Rect> FaceDetector::detectOutlines(cv_image<rgb_pixel> image) {
    std::vector<rectangle> rectangles = detector(image);
    return rectangles;
}

std::vector<FaceDetector::Point> FaceDetector::detectNoses(cv::Mat image, cv::Size viewSize) {
    TIMER_INIT;
    //const int sampleRatio = 8;

    cv::Mat scaled;
    cv::resize(image, scaled, viewSize);

    //cv::Mat sampled;
    //cv::resize(scaled, sampled, viewSize/sampleRatio);

    cv_image<rgb_pixel> dlibImage(cvIplImage(scaled));

    //std::cout << "Scaled: " << scaled.size << std::endl;
    //std::cout << "Sampled: " << sampled.size << std::endl;

    TIMER_OUTPUT("Setup")
    auto detections = this->detectOutlines(dlibImage);
//    for(auto & detection : detections) {
//        detection.set_bottom(detection.bottom() * sampleRatio);
//        detection.set_top(detection.top() * sampleRatio);
//        detection.set_left(detection.left() * sampleRatio);
//        detection.set_right(detection.right() * sampleRatio);
//    }

    TIMER_OUTPUT("Outlines")
    std::vector<FaceDetector::Point> noses(detections.size());
    TIMER_OUTPUT("Noses")
    auto noseIterator = noses.begin();
    for (const auto &face : detections){
        full_object_detection detection = pose_model(dlibImage, face);
        dlib::point noseTip = detection.part(28);
        *(noseIterator++) = FaceDetector::Point(
                noseTip.x() / (float)viewSize.width,
                noseTip.y() / (float)viewSize.height
                );
    }
    TIMER_OUTPUT("Extract tip")
    return noses;
}

