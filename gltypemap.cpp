//
// Created by obyoxar on 28.08.18.
//

#include "gltypemap.h"

#define MAP_GL_TYPE(type, gl_type) template <> GLenum toGLType<type>(){return gl_type;}

MAP_GL_TYPE(float, GL_FLOAT)
MAP_GL_TYPE(unsigned char, GL_UNSIGNED_BYTE)

#undef MAP_GL_TYPE