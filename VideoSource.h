//
// Created by obyoxar on 27.08.18.
//

#ifndef POCEEXP_VIDEOSOURCE_H
#define POCEEXP_VIDEOSOURCE_H

#include <opencv2/videoio.hpp>
#include "Buffer.h"

class VideoSource {
public:
    explicit VideoSource(cv::Size size);
    ~VideoSource();

    void refresh();
    const uchar *getData() const;

    void copyTo(void* destination) const;
    void copyTo(Buffer<uchar>& buffer, bool allowResize = false) const;

    void fetchTo(void* destination);
    void fetchTo(Buffer<uchar>& buffer, bool allowResize = false);

    size_t totalBytes() const;

    const cv::VideoCapture &getCapture() const;

    const cv::Mat getMat() const;

private:
    cv::VideoCapture capture;
    cv::Mat currentMat;
    cv::Size targetSize;
};


#endif //POCEEXP_VIDEOSOURCE_H
