//
// Created by obyoxar on 15.08.18.
//

#ifndef POCEEXP_OSHADER_H
#define POCEEXP_OSHADER_H


#include <fstream>
#include <GL/glew.h>

enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    COMPUTE_SHADER = GL_COMPUTE_SHADER
};

class OShader {
public:
    explicit OShader(std::string sourceFilePath, ShaderType type);
    bool isGood() const;
    const GLuint getGLShaderReference() const;

private:
    bool compile();
    GLuint shader;
    const std::string path;
    std::string source;
    bool good = false;
};


#endif //POCEEXP_OSHADER_H
