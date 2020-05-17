#include <utility>

#include <utility>

//
// Created by obyoxar on 15.08.18.
//

#include <iostream>
#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::ShaderProgram() : program(glCreateProgram()) {
    if(this->program == 0){
        throw std::runtime_error("Couldn't create ShaderProgram.");
    }
}

void ShaderProgram::attach(Shader shader) {
    this->shaders.push_back(shader);
    glAttachShader(this->program, shader.getGLShaderReference());
}

void ShaderProgram::attach(std::string filename, ShaderType type) {
    attach(Shader(std::move(filename), type));
}

bool ShaderProgram::link() {
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

bool ShaderProgram::isGood() {
    return this->good;
}

ShaderProgram::ShaderProgram(std::string vertex, std::string fragment) : ShaderProgram()  {
    attach(std::move(vertex), ShaderType::VERTEX_SHADER);
    attach(std::move(fragment), ShaderType::FRAGMENT_SHADER);
    link();
}

GLuint ShaderProgram::getUniformLocation(std::string name) {
    requireGood();
    GLint loc = glGetUniformLocation(this->program, name.c_str());
    if(loc < 0){
        throw std::runtime_error("Invalid name for uniform " +name+ ". Was not found or is illegal!");
    }
    return (GLuint) loc;
}

GLuint ShaderProgram::getAttribLocation(std::string name) {
    requireGood();
    GLint loc = glGetAttribLocation(this->program, name.c_str());
    if(loc < 0){
        throw std::runtime_error("Invalid name for attrib " +name+ ". Was not found or is illegal!");
    }
    return (GLuint) loc;
}


void ShaderProgram::requireGood() {
    if(!isGood()){
        throw std::runtime_error("Operation on non-good ShaderProgram");
    }
}

void ShaderProgram::use() {
    requireGood();
    glUseProgram(this->program);
}

void ShaderProgram::unUse() {
    requireGood();
    glUseProgram(0);
}

GLuint ShaderProgram::getUniformBlockIndex(std::string name) {
    requireGood();
    GLuint loc = glGetUniformBlockIndex(this->program, name.c_str());
    if(loc == GL_INVALID_INDEX){
        throw std::runtime_error("Couldn't find UniformBlock with name '" + name + "'");
    }
    return loc;
}

