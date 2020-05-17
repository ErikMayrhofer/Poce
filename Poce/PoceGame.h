//
// Created by obyoxar on 02.09.18.
//

#ifndef DLIBTEST_POCEGAME_H
#define DLIBTEST_POCEGAME_H

#include <Box2D/Box2D.h>
#include "../ShaderProgram.h"
#include "../FaceDetector.h"
#include "../VideoTexture.h"
#include "../Buffer.h"
#include "../App.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FTGL/ftgl.h>

enum GameStates : int{
    Playing = 0,
    Paused = 1,
    PlayerLWon = 2,
    PlayerRWon = 3,
    WaitingForPlayers = 4
};

struct XYUV {
    float x, y;
    float u, v;
};


struct ubo_data_game_data{
    float playerL[2];
    float playerR[2];
    float ball[2];
    float fieldSize[2];
    int timeMS;
    int gameState;
    float stateAge;
    float meterToPixelRatio;
    bool plLLost;
    bool plRLost;
    float playerLostMillis;
};

struct ubo_config_data{
    float faceSize;
    float ballSize;
    float ballEffectSize;
    float fieldWidthInM;
    float fieldWithInPixel;
    float goalAreaInPixel;
    float winTimeoutMS;
    bool allowSinglePlayer;
};

struct player_pos{
    float x;
    float y;
    bool valid;
};
struct players{
    player_pos left;
    player_pos right;
};

class PoceGame : public Game{
public:
    using Game::Game;
    void init() override;
    void loop(double deltaMs) override;

    ~PoceGame() override;

private:
    void changeState(GameStates state);
    bool isWon() const;
    bool isPaused();
    void deactivateState(GameStates state);
    void activateState(GameStates state);
    void throwIn();
    players getPlayerPos();

    ShaderProgram* program;
    FaceDetector* detector;
    VideoTexture* texture;
    Buffer<float>* vertexBuffer;
    Buffer<float>* gameDataBuffer;
    Buffer<float>* configDataBuffer;
    GLuint mvp_location;
    GLuint vpos_location;
    GLuint tex_attrib;

    XYUV vertices[4] =
            {
                    {  0.f,  0.f,  0.f,  0.f },
                    {  0.f,  1.f,  0.f,  1.f },
                    {  1.f,  0.f,  1.f,  0.f },
                    {  1.f,  1.f,  1.f,  1.f }
            };

    ubo_data_game_data game_data;
    ubo_config_data config_data = {
            100.0, //FaceSize,
            30.0, //BallSize
            400.0, //BallEffectSize
            20, //FieldWidthinM
            1000, //FieldWidthInPixel
            0,
            1000, //WinTimeout
            true,
    };

    b2Body* groundBody;
    b2Body* topBody;
    b2Body* ballBody;
    b2World* world;
    b2Body* pRBody;
    b2Body* pLBody;

    players lastPlayers;

    FTGLPixmapFont* font;
};

#endif //DLIBTEST_POCEGAME_H
