//
// Created by obyoxar on 28.08.18.
//

#include <cassert>
#include <GL/glew.h>
#include "Texture.h"

bool usedTextureUnits[OPENGL_TEXTURE_AMOUNT] = {};
GLuint GLTextureUnits[OPENGL_TEXTURE_AMOUNT] = {
        GL_TEXTURE0,  GL_TEXTURE1,  GL_TEXTURE2,  GL_TEXTURE3,  GL_TEXTURE4,  GL_TEXTURE5,  GL_TEXTURE6,  GL_TEXTURE8,  GL_TEXTURE8,  GL_TEXTURE9,
        GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15, GL_TEXTURE16, GL_TEXTURE18, GL_TEXTURE18, GL_TEXTURE19,
        GL_TEXTURE20, GL_TEXTURE21, GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26, GL_TEXTURE28, GL_TEXTURE28, GL_TEXTURE29,
        GL_TEXTURE30, GL_TEXTURE31
};

unsigned char getFreeTextureUnit(){
    for(unsigned char i = 0; i < OPENGL_TEXTURE_AMOUNT; i++){
        if(!usedTextureUnits[i]){
            return i;
        }
    }
    assert(false);
    return 0;
}

unsigned char requestFreeTextureUnit(){
    unsigned char free = getFreeTextureUnit();
    requestTextureUnit(free);
    return free;
}
#undef OPENGL_TEXTURE_AMOUNT

void freeTextureUnit(int index){
    assert(usedTextureUnits[index]);
    usedTextureUnits[index] = false;
}

void requestTextureUnit(int index){
    assert(!usedTextureUnits[index]);
    usedTextureUnits[index] = true;
}