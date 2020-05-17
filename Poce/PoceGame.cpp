//
// Created by obyoxar on 02.09.18.
//

#include "PoceGame.h"
#include <Box2D/Box2D.h>
#include "../profiler.h"

glm::mat4x4 calculateMVP(int width, int height, float vpWidth, float vpHeight){
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 p, m, mvp;
    m = glm::mat4(1.0f);
    p = glm::ortho(-0.f, vpWidth, vpHeight, -0.f, 1.f, -1.f);
    mvp = p * m;
    return mvp;
}

void PoceGame::init() {

    font = new FTGLPixmapFont("fonts/arial.ttf");
    auto error = font->Error();
    if(error){
        std::cout << "Font-Error: " << error << std::endl;
        return;
    }
    font->FaceSize(72);
    font->Render("Hello World");
    std::cout << "Hello World" << std::endl;



    struct
    {
        float x, y;
        float u, v;
    } screenVertices[4] =
            {
                    {  0.f,  0.f,  0.f,  0.f },
                    {  0.f,  1.f,  0.f,  1.f },
                    {  1.f,  0.f,  1.f,  0.f },
                    {  1.f,  1.f,  1.f,  1.f }
            };

    program = new ShaderProgram("shaders/simple.v.glsl", "shaders/simple.f.glsl");
    mvp_location = program->getUniformLocation("mvp");
    vpos_location = program->getAttribLocation("position");
    tex_attrib = program->getAttribLocation("texcoord");
    program->use();

    detector = new FaceDetector("shape_predictor_68_face_landmarks.dat");

    texture = new VideoTexture();
    texture->setUniformToUnit(program->getUniformLocation("texture_sampler"));

    vertexBuffer = new Buffer<float>(BufferType::Array, DynamicDraw);
    vertexBuffer->write(screenVertices, sizeof(screenVertices), true);

    gameDataBuffer = new Buffer<float>(BufferType::Uniform, DynamicDraw);
    gameDataBuffer->write(&game_data, sizeof(game_data), true);
    program->bindUBOTo(*gameDataBuffer, "game_data", 1);

    configDataBuffer = new Buffer<float>(BufferType::Uniform, DynamicDraw);
    configDataBuffer->write(&config_data, sizeof(config_data), true);
    program->bindUBOTo(*configDataBuffer, "config_data", 2);

    withBuffer(*vertexBuffer, {
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, vertexBuffer->getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 0));

        glEnableVertexAttribArray(tex_attrib);
        glVertexAttribPointer(tex_attrib, 2, vertexBuffer->getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 2));
    })


    //Setup Game Data
    texture->update();
    double mWidth = texture->getMat().cols;
    double mHeight = texture->getMat().rows;
    double mAspect = mWidth / mHeight; //a = w / h -> w = a * h
    double widthInM = this->config_data.fieldWidthInM;
    double heightInM = widthInM / mAspect;
    double upixelToMeterRatio = this->config_data.fieldWidthInM/this->config_data.fieldWithInPixel; //p * upm = m


    //World
    this->world = new b2World(b2Vec2(0.0f, 0.0f));
    {
        //Ground
        auto * bodyDef = new b2BodyDef();
        auto *fixtureDef = new b2FixtureDef();
        auto * shape = new b2PolygonShape();
        shape->SetAsBox(widthInM/2, 1.0f);
        bodyDef->type = b2_staticBody;
        bodyDef->position.Set(widthInM/2, heightInM+1.0f);
        bodyDef->allowSleep=false;
        fixtureDef->shape = shape;
        fixtureDef->density = 1.0f;
        fixtureDef->restitution = 1.0f;
        fixtureDef->friction = 0.0f;
        this->groundBody = this->world->CreateBody(bodyDef);
        this->groundBody->CreateFixture(fixtureDef);
    }
    {
        //Top
        auto * bodyDef = new b2BodyDef();
        auto *fixtureDef = new b2FixtureDef();
        auto * shape = new b2PolygonShape();
        bodyDef->type = b2_staticBody;
        bodyDef->position.Set(widthInM/2.f, -1.f);
        bodyDef->allowSleep=false;
        shape->SetAsBox(widthInM/2, 1.f);
        fixtureDef->shape = shape;
        fixtureDef->density = 1.0f;
        fixtureDef->restitution = 1.0f;
        fixtureDef->friction = 0.0f;

        this->groundBody = this->world->CreateBody(bodyDef);
        this->groundBody->CreateFixture(fixtureDef);
    }
    {
        //Ball
        auto *bodyDef = new b2BodyDef();
        auto *shape = new b2CircleShape();
        auto *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_dynamicBody;
        bodyDef->position.Set(widthInM / 2, heightInM / 2);
        bodyDef->linearDamping = 0.0f;
        bodyDef->allowSleep=false;
        bodyDef->bullet=true;
        shape->m_radius = this->config_data.ballSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 100.f;
        fixtureDef->friction = 0.f;
        fixtureDef->restitution = 1.f;
        this->ballBody = this->world->CreateBody(bodyDef);
        this->ballBody->CreateFixture(fixtureDef);
    }
    { //TODO Force sync with Faces at startup
        //Player1
        auto *bodyDef = new b2BodyDef();
        auto *shape = new b2CircleShape();
        auto *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_kinematicBody;
        bodyDef->position.Set(0, 0);
        shape->m_radius = this->config_data.faceSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 20.f;
        fixtureDef->friction = 0.0f;
        fixtureDef->restitution = 1.0f;
        this->pRBody = this->world->CreateBody(bodyDef);
        this->pRBody->CreateFixture(fixtureDef);
    }
    {
        //Player2
        auto *bodyDef = new b2BodyDef();
        auto *shape = new b2CircleShape();
        auto *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_kinematicBody;
        bodyDef->position.Set(0, 0);
        shape->m_radius = this->config_data.faceSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 20.f;
        fixtureDef->friction = 0.0f;
        fixtureDef->restitution = 1.0f;
        this->pLBody = this->world->CreateBody(bodyDef);
        this->pLBody->CreateFixture(fixtureDef);
    }

    this->throwIn();
}

void PoceGame::loop(double deltaMS) {
    TIMER_INIT
    TIMER_OUTPUT("Start")

    std::cout << "FPS: " << 1000/deltaMS << std::endl;
    this->game_data.stateAge += static_cast<float>(deltaMS);
    std::cout << "Loop: " << this->game_data.gameState << " - " << this->game_data.stateAge << std::endl;
    cv::Size size = getApp()->getWindow()->getSize();

    if(isWon() && this->game_data.stateAge > this->config_data.winTimeoutMS){
        this->throwIn();
    }

    TIMER_OUTPUT("Game Data")

    texture->update();
    float wWidth = size.width;
    float wHeight = size.height;
    float wAspect = wWidth/wHeight; //a = w/h ----- w = a * h ----- h = w / a
    float mWidth = static_cast<float>(texture->getMat().cols);
    float mHeight = static_cast<float>(texture->getMat().rows);
    float mAspect = mWidth / mHeight;
    float cWidth = this->config_data.fieldWithInPixel;
    float cHeight = cWidth / mAspect;

    float meterToPixelRatio = cWidth / config_data.fieldWidthInM; //meter * r = pixel

    vertices[1].y = cHeight;
    vertices[2].x = cWidth;
    vertices[3].x = cWidth;
    vertices[3].y = cHeight;
    vertexBuffer->write(vertices, sizeof(vertices), true);
    game_data.fieldSize[0] = cWidth;
    game_data.fieldSize[1] = cHeight;
    game_data.meterToPixelRatio = meterToPixelRatio;
    game_data.timeMS = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count() / 1000000);

    float vWidth = cWidth;
    float vHeight = vWidth / wAspect;
    if(vHeight < cHeight){
        vHeight = cHeight;
        vWidth = vHeight * wAspect;
    }
    if(vWidth < cWidth){
        vWidth = cWidth;
        vHeight = vWidth / wAspect;
    }

    TIMER_OUTPUT("Calc Physix")

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(calculateMVP(size.width, size.height, vWidth, vHeight)));


    TIMER_OUTPUT("Push Matrix")
    players plr = getPlayerPos();
    TIMER_OUTPUT("Playerpos")

    double deltaS = deltaMS/1000.0f;
    double fieldHeightInM = config_data.fieldWidthInM/mAspect;
    if(plr.right.valid){
        float plrX = plr.right.x * cWidth;
        float plrY = plr.right.y * cHeight;
        auto sx = static_cast<float>(
                (plr.right.x * config_data.fieldWidthInM - lastPlayers.right.x * config_data.fieldWidthInM) / deltaS);
        auto sy = static_cast<float>((plr.right.y * fieldHeightInM - lastPlayers.right.y * fieldHeightInM) / deltaS);

        if(this->game_data.plRLost){
            this->pRBody->SetTransform(b2Vec2(plrX/meterToPixelRatio, plrY/meterToPixelRatio),
                    this->pRBody->GetAngle());
            this->pRBody->SetLinearVelocity(b2Vec2(0, 0));
        }else{
            this->pRBody->SetLinearVelocity(b2Vec2(sx, sy));
        }
        this->game_data.plRLost = false;
    }else{
        this->game_data.plRLost = true;
    }
    if(plr.left.valid){
        float plrX = plr.left.x * cWidth;
        float plrY = plr.left.y * cHeight;
        auto sx = static_cast<float>(
                (plr.left.x * config_data.fieldWidthInM - lastPlayers.left.x * config_data.fieldWidthInM) / deltaS);
        auto sy = static_cast<float>((plr.left.y * fieldHeightInM - lastPlayers.left.y * fieldHeightInM) / deltaS);
        if(this->game_data.plLLost){
            this->pLBody->SetTransform(b2Vec2(plrX/meterToPixelRatio, plrY/meterToPixelRatio),
                                       this->pLBody->GetAngle());
            this->pLBody->SetLinearVelocity(b2Vec2(0, 0));
        }else{
            this->pLBody->SetLinearVelocity(b2Vec2(sx, sy));
        }
        this->game_data.plLLost = false;
    }else{
        this->game_data.plLLost = true;
    }
    this->lastPlayers = plr;
    if(!plr.left.valid || !plr.right.valid){
        this->game_data.playerLostMillis+=static_cast<float>(deltaMS);
    }else{
        this->game_data.playerLostMillis = 0.0;
    }
    if(this->game_data.playerLostMillis > 500){
        if(!this->config_data.allowSinglePlayer) {
            this->changeState(GameStates::WaitingForPlayers);
        }
    }

    this->world->Step(static_cast<float32>(deltaMS / 1000), 4, 2);
    game_data.ball[0] = this->ballBody->GetPosition().x * meterToPixelRatio;
    game_data.ball[1] = this->ballBody->GetPosition().y * meterToPixelRatio;
    game_data.playerR[0] = this->pRBody->GetPosition().x* meterToPixelRatio;
    game_data.playerR[1] = this->pRBody->GetPosition().y* meterToPixelRatio;
    game_data.playerL[0] = this->pLBody->GetPosition().x* meterToPixelRatio;
    game_data.playerL[1] = this->pLBody->GetPosition().y* meterToPixelRatio;


    TIMER_OUTPUT("Control")

    if(this->game_data.gameState == GameStates::Playing) {
        if (game_data.ball[0] < config_data.goalAreaInPixel) {
            this->changeState(GameStates::PlayerLWon);
        } else if (game_data.ball[0] > config_data.fieldWithInPixel - config_data.goalAreaInPixel) {
            this->changeState(GameStates::PlayerRWon);
        }
    }else if(this->game_data.gameState == GameStates::WaitingForPlayers){
        if(plr.left.valid && plr.right.valid){
            this->changeState(GameStates::Playing);
        }
    }


    TIMER_OUTPUT("Logic")


    gameDataBuffer->write(&game_data, sizeof(game_data), true);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
    font->Render("Hello World");
    std::stringstream s;
    s << "Hello World" << deltaMS;
    font->Render(s.str().c_str());
    program->use();


    TIMER_OUTPUT("Render")
}

PoceGame::~PoceGame() {
    delete program;
    delete detector;
    delete texture;
    delete vertexBuffer;
    delete font;
    delete gameDataBuffer;
}

void PoceGame::changeState(GameStates state) {
    std::cout << "Changing state to: " << state << std::endl;
    this->game_data.gameState = state;
    this->game_data.stateAge = 0;
}

bool PoceGame::isWon() const {
    return this->game_data.gameState == GameStates::PlayerRWon ||
            this->game_data.gameState == GameStates::PlayerLWon;
}

bool PoceGame::isPaused() {
    return this->game_data.gameState == GameStates::Paused || isWon();
}

void PoceGame::throwIn() {
    std::cout << "Trow in" << std::endl;
    this->ballBody->SetTransform(b2Vec2(
            this->game_data.fieldSize[0]/2/this->game_data.meterToPixelRatio,
            this->game_data.fieldSize[1]/2/this->game_data.meterToPixelRatio
                               ),
                               this->ballBody->GetAngle());
    this->ballBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    changeState(GameStates::Playing);
    this->game_data.plRLost = true;
    this->game_data.plLLost = true;

}

players PoceGame::getPlayerPos() {
    TIMER_INIT
    players plrs = this->lastPlayers;
    plrs.right.valid = false;
    plrs.left.valid = false;
    TIMER_OUTPUT("Setup")
    std::vector<FaceDetector::Point> noses = detector->detectNoses(texture->getMat(), cv::Size(640, 480));
    TIMER_OUTPUT("Detector")
    for (auto &nose : noses) {
        if (nose.x < 0.5) {
            plrs.left.x = static_cast<float>(nose.x);
            plrs.left.y = static_cast<float>(nose.y);
            plrs.left.valid = true;
        } else {
            plrs.right.x = static_cast<float>(nose.x);
            plrs.right.y = static_cast<float>(nose.y);
            plrs.right.valid = true;
        }
    }
    TIMER_OUTPUT("Side-Choose")
    return plrs;
}

void PoceGame::deactivateState(GameStates state) {
    switch(state){
        case GameStates::Playing:
            this->ballBody->SetType(b2BodyType::b2_staticBody);
            break;
        case Paused:break;
        case PlayerLWon:break;
        case PlayerRWon:break;
        case WaitingForPlayers:break;
    }
}

void PoceGame::activateState(GameStates state) {
    switch(state){
        case GameStates::Playing:
            this->ballBody->SetType(b2BodyType::b2_dynamicBody);
            break;
        case Paused: break;
        case PlayerLWon:break;
        case PlayerRWon:break;
        case WaitingForPlayers:break;
    }
}