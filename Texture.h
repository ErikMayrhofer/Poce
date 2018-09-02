//
// Created by obyoxar on 28.08.18.
//

#ifndef POCEEXP_TEXTURE_H
#define POCEEXP_TEXTURE_H

#include <GL/glew.h>
#include "Buffer.h"
#include <cassert>
#include "gltypemap.h"

#define OPENGL_TEXTURE_AMOUNT 32

extern GLuint GLTextureUnits[OPENGL_TEXTURE_AMOUNT];

enum class ColorMode {
    RGB = GL_RGB
};

enum class ParameterName {
    DepthStencilTextureMode = GL_DEPTH_STENCIL_TEXTURE_MODE,
    BaseLevel = GL_TEXTURE_BASE_LEVEL,
    CompareFunc = GL_TEXTURE_COMPARE_FUNC,
    CompareMode = GL_TEXTURE_COMPARE_MODE,
    LodBias = GL_TEXTURE_LOD_BIAS,
    MinFilter = GL_TEXTURE_MIN_FILTER,
    MagFilter = GL_TEXTURE_MAG_FILTER,
    MinLod = GL_TEXTURE_MIN_LOD,
    MaxLod = GL_TEXTURE_MAX_LOD,
    MaxLevel = GL_TEXTURE_MAX_LEVEL,
    SwizzleRed = GL_TEXTURE_SWIZZLE_R,
    SwizzleGreen = GL_TEXTURE_SWIZZLE_G,
    SwizzleBlue = GL_TEXTURE_SWIZZLE_B,
    SwizzleAlpha = GL_TEXTURE_SWIZZLE_A,
    WrapS = GL_TEXTURE_WRAP_S,
    WrapT = GL_TEXTURE_WRAP_T,
    WrapR = GL_TEXTURE_WRAP_R
};

enum class DepthStencilTextureMode {
    Depth = GL_DEPTH_COMPONENT,
    Stencil = GL_STENCIL_COMPONENTS
};

enum class WrapMode {
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    Repeat = GL_REPEAT,
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
};

enum class MinFilter{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class MagFilter{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR
};

unsigned char requestFreeTextureUnit();
void freeTextureUnit(int index);
void requestTextureUnit(int index);
unsigned char getFreeTextureUnit();

template <typename T>
class Texture {
public:
    Texture(ColorMode mode);
    Texture(ColorMode mode, unsigned char textureUnit);
    ~Texture();

    void bind();
    void unbind();

    void useBuffer(Buffer<T>& buffer, uint width, uint height);

    inline GLenum getGLDataType() const;

    //void setParameter(ParameterName name, GLuint value);

    void setWrapS(WrapMode value);
    void setWrapT(WrapMode value);
    void setWrapR(WrapMode value);
    inline void setWrapST(WrapMode value);
    inline void setWrapSTR(WrapMode value);
    void setMinFilter(MinFilter value);
    void setMagFilter(MagFilter value);
    void setMinMagFilter(MagFilter value);
    inline void setUniformToUnit(GLuint uniformName);
    inline GLuint getGLTextureUnit();

private:
    ColorMode colorMode;
    unsigned char textureUnit;
    GLuint textureHandle;
};

template<typename T>
Texture<T>::Texture(ColorMode mode) :
        Texture(mode, getFreeTextureUnit()) {}

template<typename T>
Texture<T>::Texture(ColorMode mode, unsigned char textureUnit) :
colorMode(mode), textureUnit(textureUnit), textureHandle(0) {
    requestTextureUnit(textureUnit);
    glGenTextures(1, &textureHandle);
    assert(textureHandle != 0);
}



template<typename T>
void Texture<T>::bind() {
    glActiveTexture(getGLTextureUnit(), textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
}

template<typename T>
Texture<T>::~Texture() {
    freeTextureUnit(textureUnit);
    glDeleteTextures(1, &textureHandle);
}

template<typename T>
void Texture<T>::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

template<typename T>
void Texture<T>::useBuffer(Buffer<T> &buffer, uint width, uint height) {
    withBuffer(buffer, {
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint) colorMode, width, height, 0 , (GLenum) colorMode, getGLDataType(), nullptr);
    })
}

template<typename T>
GLenum Texture<T>::getGLDataType() const {
    return toGLType<T>();
}

#define PARAM_SETTER(localname, glname, argumenttype, settertype) \
template<typename T> \
void Texture<T>::localname(argumenttype value) { \
    glTexParameter##settertype(GL_TEXTURE_2D, glname, (GLint) value); \
}

PARAM_SETTER(setWrapS, GL_TEXTURE_WRAP_S, WrapMode, i)
PARAM_SETTER(setWrapT, GL_TEXTURE_WRAP_T, WrapMode, i)
PARAM_SETTER(setWrapR, GL_TEXTURE_WRAP_R, WrapMode, i)

template<typename T>
void Texture<T>::setWrapST(WrapMode value) {
    setWrapS(value);
    setWrapT(value);
}

PARAM_SETTER(setMinFilter, GL_TEXTURE_MIN_FILTER, MinFilter, i)
PARAM_SETTER(setMagFilter, GL_TEXTURE_MIN_FILTER, MagFilter, i)

template<typename T>
void Texture<T>::setWrapSTR(WrapMode value) {
    setWrapST(value);
    setWrapR(value);
}

template<typename T>
void Texture<T>::setMinMagFilter(MagFilter value) {
    setMinFilter((MinFilter)value);
    setMagFilter(value);
}

template<typename T>
GLuint Texture<T>::getGLTextureUnit() {
    return GLTextureUnits[textureUnit];
}

template<typename T>
void Texture<T>::setUniformToUnit(GLuint uniformName) {
    glUniform1i(uniformName, textureUnit);
}


#undef PARAM_SETTER

#endif //POCEEXP_TEXTURE_H
