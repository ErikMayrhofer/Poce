//
// Created by obyoxar on 02.09.18.
//

#ifndef DLIBTEST_POCEGAME_H
#define DLIBTEST_POCEGAME_H


#include "../ShaderProgram.h"
#include "../FaceDetector.h"
#include "../VideoTexture.h"
#include "../Buffer.h"
#include "../App.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct xyuv {
    float x, y;
    float u, v;
};


struct ubo_data_game_data{
    float playerL[2];
    float playerR[2];
    float ball[2];
};

class PoceGame : public Game{
public:
    using Game::Game;
    void init() override;
    void loop() override;

    ~PoceGame() override;
private:
    ShaderProgram* program;
    FaceDetector* detector;
    VideoTexture* texture;
    Buffer<float>* vertexBuffer;
    Buffer<float>* gameDataBuffer;
    GLuint mvp_location;
    GLuint vpos_location;
    GLuint tex_attrib;

    xyuv vertices[4] =
            {
                    {  0.f,  0.f,  0.f,  0.f },
                    {  0.f,  1.f,  0.f,  1.f },
                    {  1.f,  0.f,  1.f,  0.f },
                    {  1.f,  1.f,  1.f,  1.f }
            };

    ubo_data_game_data game_data;
};

#endif //DLIBTEST_POCEGAME_H
