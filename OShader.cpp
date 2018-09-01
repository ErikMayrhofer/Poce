#include <utility>

//
// Created by obyoxar on 15.08.18.
//

#include <iostream>
#include "OShader.h"

OShader::OShader(std::string sourceFilePath, ShaderType type) :
    path(std::move(sourceFilePath)) {

    std::ifstream ifs(this->path);
    if(ifs.fail()){
        std::cerr << "Couldn't open " << this->path << std::endl;
    }else if(!ifs.good()){
        std::cerr << "Error while opening " << this->path << std::endl;
    }
    this->source = std::string( (std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));

    this->shader = glCreateShader(type);
    const char* src = this->source.c_str();
    glShaderSource(shader, 1, &src, nullptr);

    good = compile();
}

bool OShader::compile() {
    glCompileShader(this->shader);
    GLint success = 0;
    glGetShaderiv(this->shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE){
        std::cerr << "Shader compilation of '" << this->path << "' failed." << std::endl;
        GLint logSize = 0;
        glGetShaderiv(this->shader, GL_INFO_LOG_LENGTH, &logSize);
        GLsizei realLength;
        char log[logSize];

        glGetShaderInfoLog(this->shader, logSize, &realLength, log);

        std::cerr << "Log:" << std::endl;
        std::cerr << log << std::endl;

        glDeleteShader(this->shader);
        return false;
    }
    return true;
}

bool OShader::isGood() const {
    return this->good;
}

const GLuint OShader::getGLShaderReference() const {
    return this->shader;
}
