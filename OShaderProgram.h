#include <utility>

//
// Created by obyoxar on 15.08.18.
//

#ifndef POCEEXP_OSHADERPROGRAM_H
#define POCEEXP_OSHADERPROGRAM_H


#include <string>
#include <vector>
#include "OShader.h"
#include "Buffer.h"

class OShaderProgram {
public:
    OShaderProgram();
    OShaderProgram(std::string vertex, std::string fragment);
    void attach(std::string filename, ShaderType type);
    bool link();
    bool isGood();

    GLuint getUniformLocation(std::string name);
    GLuint getAttribLocation(std::string name);
    GLuint getUniformBlockIndex(std::string name);

    template<typename T>
    void bindUBOTo(Buffer<T> &ubo, std::string name, GLuint bindingPointIndex);


    void use();

private:
    void requireGood();
    void attach(OShader shader);
    GLuint program;
    std::vector<OShader> shaders;
    bool good;
};

template<typename T>
void OShaderProgram::bindUBOTo(Buffer<T> &ubo, std::string name, GLuint bindingPointIndex) {
    assert(ubo.getType() == Uniform);
    GLuint block_index = this->getUniformBlockIndex(std::move(name));
    ubo.bind(bindingPointIndex);
    glUniformBlockBinding(this->program, block_index, bindingPointIndex);
}

#endif //POCEEXP_OSHADERPROGRAM_H
