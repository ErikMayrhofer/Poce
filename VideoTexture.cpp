//
// Created by obyoxar on 28.08.18.
//

#include "VideoTexture.h"

VideoTexture::VideoTexture():
videoSource(),
pixelBuffer(BufferType::PixelUnpack, DynamicDraw),
texture(ColorMode::RGB){
    texture.setWrapST(WrapMode::Repeat);
    texture.setMinMagFilter(MagFilter::Linear);
}

void VideoTexture::update() {
    videoSource.fetchTo(pixelBuffer, firstFill);
    texture.useBuffer(pixelBuffer, static_cast<uint>(videoSource.getMat().cols),
                      static_cast<uint>(videoSource.getMat().rows));

    firstFill = false;
}

void VideoTexture::setUniformToUnit(GLuint uniformLoc) {
    texture.setUniformToUnit(uniformLoc);
}

const cv::Mat VideoTexture::getMat() const {
    return videoSource.getMat();
}

VideoTexture::~VideoTexture() = default;
