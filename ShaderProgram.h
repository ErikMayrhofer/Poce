#include <utility>

//
// Created by obyoxar on 15.08.18.
//

#ifndef POCEEXP_O_SHADER_PROGRAM_H
#define POCEEXP_O_SHADER_PROGRAM_H


#include <string>
#include <vector>
#include "Shader.h"
#include "Buffer.h"

class ShaderProgram {
public:
    ShaderProgram();
    ShaderProgram(std::string vertex, std::string fragment);
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
    void attach(Shader shader);
    GLuint program;
    std::vector<Shader> shaders;
    bool good;
};

template<typename T>
void ShaderProgram::bindUBOTo(Buffer<T> &ubo, std::string name, GLuint bindingPointIndex) {
    assert(ubo.getType() == Uniform);
    GLuint block_index = this->getUniformBlockIndex(std::move(name));
    ubo.bind(bindingPointIndex);
    glUniformBlockBinding(this->program, block_index, bindingPointIndex);
}

#endif //POCEEXP_O_SHADER_PROGRAM_H
