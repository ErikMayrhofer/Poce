//
// Created by obyoxar on 19/12/18.
//

#ifndef POCEEXP_TESTTEST_H
#define POCEEXP_TESTTEST_H


#include <FTGL/ftgl.h>
#include "../App.h"

class TestTest : public Game{
public:
    TestTest(App *app);

    void init() override;

    void loop(double deltaMillis) override;

private:
    FTGLPixmapFont* font;

};


#endif //POCEEXP_TESTTEST_H
