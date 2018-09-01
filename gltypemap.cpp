//
// Created by obyoxar on 28.08.18.
//

#include "gltypemap.h"

#define MAPGLTYPE(type, gltype) template <> GLenum toGLType<type>(){return gltype;}

MAPGLTYPE(float, GL_FLOAT)
MAPGLTYPE(unsigned char, GL_UNSIGNED_BYTE)

#undef MAPGLTYPE