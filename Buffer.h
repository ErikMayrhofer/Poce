//
// Created by obyoxar on 27.08.18.
//

#ifndef POCEEXP_BUFFER_H
#define POCEEXP_BUFFER_H

#include <GL/glew.h>
#include <opencv2/core/hal/interface.h>
#include <cstring>
#include "gltypemap.h"
#include <cassert>

#define withBuffer(buffer, code) {\
(buffer).bind(); \
{code} \
(buffer).unbind(); \
}

enum BufferType {
    Array = GL_ARRAY_BUFFER,
    PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
    /*AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
    CopyRead = GL_COPY_READ_BUFFER,
    CopyWrite = GL_COPY_WRITE_BUFFER,
    DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
    DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
    ElementArray = GL_ELEMENT_ARRAY_BUFFER,
    PixelPack = GL_PIXEL_PACK_BUFFER,

    Query = GL_QUERY_BUFFER,
    ShaderStorage = GL_SHADER_STORAGE_BUFFER,
    Texture = GL_TEXTURE_BUFFER,
    TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,*/
    Uniform = GL_UNIFORM_BUFFER,
};

enum MapMode {
    ReadOnly = GL_READ_ONLY,
    WriteOnly = GL_WRITE_ONLY,
    ReadWrite = GL_READ_WRITE
};

enum BufferMode {
    StreamDraw = GL_STREAM_DRAW,
    StreamRead = GL_STREAM_READ,
    StreamCopy = GL_STREAM_COPY,
    StaticDraw = GL_STATIC_DRAW,
    StaticRead = GL_STATIC_READ,
    StaticCopy = GL_STATIC_COPY,
    DynamicDraw = GL_DYNAMIC_DRAW,
    DynamicRead = GL_DYNAMIC_READ,
    DynamicCopy = GL_DYNAMIC_COPY
};

template <typename U>
class Buffer {
public:
    explicit Buffer(BufferType type, BufferMode mode);
    ~Buffer();

    void write(const void* data, size_t amount, bool allowResize = true, void* offset = nullptr);

    void bind() const;
    void bind(GLuint index) const;
    void unbind() const;
    template<typename T>
    T *map(MapMode mode);

    void unmap();

    GLuint getBufferHandle() const;


    inline GLuint getGLDataType(){
        return toGLType<U>();
    }

    BufferType getType() const;

private:
    BufferType type;
    GLuint bufferHandle;
    BufferMode mode;
};


template <typename U>
Buffer<U>::Buffer(BufferType type, BufferMode mode): type(type), mode(mode), bufferHandle(0) {
    glGenBuffers(1, &bufferHandle);
}

template <typename U>
void Buffer<U>::write(const void *data, size_t amount, bool allowResize, void *) {
    if(allowResize){
        this->bind();
        glBufferData(this->type, amount, data, this->mode);
        this->unbind();
    }else{
        auto pbomap = this->map<void>(MapMode::WriteOnly);
        memcpy(pbomap, data, amount);
        unmap();
    }
}

template <typename U>
void Buffer<U>::bind() const {
    glBindBuffer(this->type, this->bufferHandle);
}

template <typename U>
void Buffer<U>::unbind() const {
    glBindBuffer(this->type, 0);
}

template <typename U>
template<typename T>
T *Buffer<U>::map(MapMode mode) {
    this->bind();
    void* pointer = glMapBuffer(this->type, mode);
    return static_cast<T*>(pointer);
}

template <typename U>
void Buffer<U>::unmap() {
    this->bind();
    glUnmapBuffer(this->type);
}

template <typename U>
Buffer<U>::~Buffer() {
    glDeleteBuffers(1, &this->bufferHandle);
}

template <typename U>
GLuint Buffer<U>::getBufferHandle() const {
    return bufferHandle;
}

template<typename U>
void Buffer<U>::bind(GLuint index) const {
    assert(this->type == Uniform);
    glBindBufferBase(this->type, index, this->bufferHandle);
}

template<typename U>
BufferType Buffer<U>::getType() const {
    return type;
}

#endif //POCEEXP_BUFFER_H
