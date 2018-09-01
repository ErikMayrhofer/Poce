#include <utility>

#include <utility>

//
// Created by obyoxar on 15.08.18.
//

#include <iostream>
#include "OShaderProgram.h"
#include <iostream>

OShaderProgram::OShaderProgram() : program(glCreateProgram()) {
    if(this->program == 0){
        throw std::runtime_error("Couldn't create ShaderProgram.");
    }
}

void OShaderProgram::attach(OShader shader) {
    this->shaders.push_back(shader);
    glAttachShader(this->program, shader.getGLShaderReference());
}

void OShaderProgram::attach(std::string filename, ShaderType type) {
    attach(OShader(std::move(filename), type));
}

bool OShaderProgram::link() {
    glLinkProgram(program);
    GLint link_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if(link_ok != GL_TRUE){
        std::cerr << "Program linking failed!" << std::endl;
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
        GLsizei realLength;
        char log[logSize];

        glGetProgramInfoLog(program, logSize, &realLength, log);

        std::cerr << log << std::endl;

        glDeleteProgram(program);
        this->good = false;
    }
    this->good = true;
    return isGood();
}

bool OShaderProgram::isGood() {
    return this->good;
}

OShaderProgram::OShaderProgram(std::string vertex, std::string fragment) : OShaderProgram()  {
    attach(std::move(vertex), ShaderType::VERTEX_SHADER);
    attach(std::move(fragment), ShaderType::FRAGMENT_SHADER);
    link();
}

GLuint OShaderProgram::getUniformLocation(std::string name) {
    requireGood();
    GLint loc = glGetUniformLocation(this->program, name.c_str());
    if(loc < 0){
        throw std::runtime_error("Invalid name for uniform " +name+ ". Was not found or is illegal!");
    }
    return (GLuint) loc;
}

GLuint OShaderProgram::getAttribLocation(std::string name) {
    requireGood();
    GLint loc = glGetAttribLocation(this->program, name.c_str());
    if(loc < 0){
        throw std::runtime_error("Invalid name for attrib " +name+ ". Was not found or is illegal!");
    }
    return (GLuint) loc;
}


void OShaderProgram::requireGood() {
    if(!isGood()){
        throw std::runtime_error("Operation on non-good ShaderProgram");
    }
}

void OShaderProgram::use() {
    requireGood();
    glUseProgram(this->program);
}

GLuint OShaderProgram::getUniformBlockIndex(std::string name) {
    requireGood();
    GLuint loc = glGetUniformBlockIndex(this->program, name.c_str());
    if(loc == GL_INVALID_INDEX){
        throw std::runtime_error("Couldn't find UniformBlock with name '" + name + "'");
    }
    return loc;
}

