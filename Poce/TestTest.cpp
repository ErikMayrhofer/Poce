//
// Created by obyoxar on 19/12/18.
//

#include "TestTest.h"

#include <FTGL/ftgl.h>

TestTest::TestTest(App *app) : Game(app) {

}

void TestTest::init() {
    font = new FTGLPixmapFont("fonts/arial.ttf");

    auto error = font->Error();

    if(error){
        std::cout << "Font-Error: " << error << std::endl;
    }

    font->FaceSize(72);
    font->Render("Hello World");

    std::cout << "Hello World" << std::endl;
}

void TestTest::loop(double deltaMillis) {
    font->Render("Hello World");
}
