//
// Created by obyoxar on 27.08.18.
//

#include "VideoSource.h"
#include <cstring>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

VideoSource::VideoSource() : capture(0){
    //TODO Don't hardcode
    capture.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    assert(this->capture.isOpened());
}

VideoSource::~VideoSource() {
    capture.release();
}

void VideoSource::refresh() {
    this->capture >> this->currentMat;
    cv::cvtColor(this->currentMat, this->currentMat, cv::COLOR_BGR2RGB);
}

const uchar *VideoSource::getData() const {
    return this->currentMat.data;
}

void VideoSource::copyTo(void *destination) const {
    std::memcpy(destination, getData(), this->totalBytes());
}

size_t VideoSource::totalBytes() const {
    return this->currentMat.total() * this->currentMat.elemSize();
}

void VideoSource::copyTo(Buffer<uchar> &buffer, bool allowResize) const {
    buffer.write(this->getData(), this->totalBytes(), allowResize);
}

void VideoSource::fetchTo(void *destination) {
    refresh();
    copyTo(destination);
}

void VideoSource::fetchTo(Buffer<uchar> &buffer, bool allowResize) {
    refresh();
    copyTo(buffer, allowResize);
}

const cv::VideoCapture &VideoSource::getCapture() const {
    return capture;
}

const cv::Mat VideoSource::getMat() const {
    return currentMat;
}
