//
// Created by obyoxar on 28.08.18.
//

#include "VideoTexture.h"

VideoTexture::VideoTexture(unsigned int width, unsigned int height)
:VideoTexture(cv::Size(width, height)) {

}

VideoTexture::VideoTexture(cv::Size targetSize):
videoSource(targetSize),
pixelBuffer(BufferType::PixelUnpack, DynamicDraw),
texture(static_cast<unsigned int>(targetSize.width), static_cast<unsigned int>(targetSize.height), ColorMode::RGB){
    texture.setWrapST(WrapMode::Repeat);
    texture.setMinMagFilter(MagFilter::Linear);
}

void VideoTexture::update() {
    videoSource.fetchTo(pixelBuffer, firstFill);
    texture.useBuffer(pixelBuffer);

    firstFill = false;
}

void VideoTexture::setUniformToUnit(GLuint uniformLoc) {
    texture.setUniformToUnit(uniformLoc);
}

const cv::Mat VideoTexture::getMat() const {
    return videoSource.getMat();
}

VideoTexture::~VideoTexture() = default;
