//
// Created by obyoxar on 02.09.18.
//

#include "PoceGame.h"

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
    struct
    {
        float x, y;
        float u, v;
    } vertices[4] =
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
    vertexBuffer->write(vertices, sizeof(vertices), true);

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
    float mWidth = texture->getMat().cols;
    float mHeight = texture->getMat().rows;
    float mAspect = mWidth / mHeight; //a = w / h -> w = a * h
    float widthInM = this->config_data.fieldWidthInM;
    float heightInM = widthInM / mAspect;
    float upixelToMeterRatio = this->config_data.fieldWidthInM/this->config_data.fieldWithInPixel; //p * upm = m

    //World
    this->world = new b2World(b2Vec2(0.0f, 0.0f));
    {
        //Ground
        b2BodyDef* bodyDef = new b2BodyDef();
        b2FixtureDef *fixtureDef = new b2FixtureDef();
        b2PolygonShape* shape = new b2PolygonShape();
        bodyDef->type = b2_staticBody;
        bodyDef->position.Set(widthInM/2, heightInM+1.0f);
        shape->SetAsBox(widthInM/2, 1.0f);
        fixtureDef->shape = shape;
        fixtureDef->restitution = 1.0f;
        this->groundBody = this->world->CreateBody(bodyDef);
        this->groundBody->CreateFixture(fixtureDef);
    }
    {
        //Top
        b2BodyDef* bodyDef = new b2BodyDef();
        b2FixtureDef *fixtureDef = new b2FixtureDef();
        b2PolygonShape* shape = new b2PolygonShape();
        bodyDef->type = b2_staticBody;
        bodyDef->position.Set(widthInM/2.f, -1.f);
        shape->SetAsBox(widthInM/2, 1.f);
        fixtureDef->shape = shape;
        fixtureDef->restitution = 1.0f;
        this->groundBody = this->world->CreateBody(bodyDef);
        this->groundBody->CreateFixture(fixtureDef);
    }
    {
        //Ball
        b2BodyDef *bodyDef = new b2BodyDef();
        b2CircleShape *shape = new b2CircleShape();
        b2FixtureDef *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_dynamicBody;
        bodyDef->position.Set(widthInM / 2, heightInM / 2);
        bodyDef->linearDamping = 0.1f;
        shape->m_radius = this->config_data.ballSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 20.f;
        fixtureDef->friction = 0.3f;
        fixtureDef->restitution = 1.0f;
        this->ballBody = this->world->CreateBody(bodyDef);
        this->ballBody->CreateFixture(fixtureDef);
    }
    { //TODO Force sync with Faces at startup
        //Player1
        b2BodyDef *bodyDef = new b2BodyDef();
        b2CircleShape *shape = new b2CircleShape();
        b2FixtureDef *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_kinematicBody;
        bodyDef->position.Set(0, 0);
        shape->m_radius = this->config_data.faceSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 20.f;
        fixtureDef->friction = 0.3f;
        this->pRBody = this->world->CreateBody(bodyDef);
        this->pRBody->CreateFixture(fixtureDef);
    }
    {
        //Player2
        b2BodyDef *bodyDef = new b2BodyDef();
        b2CircleShape *shape = new b2CircleShape();
        b2FixtureDef *fixtureDef = new b2FixtureDef();
        bodyDef->type = b2_kinematicBody;
        bodyDef->position.Set(0, 0);
        shape->m_radius = this->config_data.faceSize*upixelToMeterRatio;
        fixtureDef->shape = shape;
        fixtureDef->density = 20.f;
        fixtureDef->friction = 0.3f;
        this->pLBody = this->world->CreateBody(bodyDef);
        this->pLBody->CreateFixture(fixtureDef);
    }
    this->throwIn();
}

void PoceGame::loop(double deltaMS) {
    this->game_data.stateAge += deltaMS;
    std::cout << "Loop: " << this->game_data.gameState << " - " << this->game_data.stateAge << std::endl;
    cv::Size size = getApp()->getWindow()->getSize();

    if(isWon() && this->game_data.stateAge > this->config_data.winTimeoutMS){
        this->throwIn();
    }

    texture->update();
    float wWidth = size.width;
    float wHeight = size.height;
    float wAspect = wWidth/wHeight; //a = w/h ----- w = a * h ----- h = w / a
    float mWidth = texture->getMat().cols;
    float mHeight = texture->getMat().rows;
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

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(calculateMVP(size.width, size.height, vWidth, vHeight)));


    players plr = getPlayerPos();
    double deltaS = deltaMS/1000.0f;
    double fieldHeightInM = config_data.fieldWidthInM/mAspect;
    if(plr.right.valid){
        float plrX = plr.right.x * cWidth;
        float plrY = plr.right.y * cHeight;
        double sx = (plr.right.x*config_data.fieldWidthInM - lastPlayers.right.x*config_data.fieldWidthInM)/deltaS;
        double sy = (plr.right.y*fieldHeightInM - lastPlayers.right.y*fieldHeightInM)/deltaS;

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
        double sx = (plr.left.x*config_data.fieldWidthInM - lastPlayers.left.x*config_data.fieldWidthInM)/deltaS;
        double sy = (plr.left.y*fieldHeightInM - lastPlayers.left.y*fieldHeightInM)/deltaS;
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


    this->world->Step(static_cast<float32>(deltaMS / 1000), 6, 2);
    game_data.ball[0] = this->ballBody->GetPosition().x * meterToPixelRatio;
    game_data.ball[1] = this->ballBody->GetPosition().y * meterToPixelRatio;
    game_data.playerR[0] = this->pRBody->GetPosition().x* meterToPixelRatio;
    game_data.playerR[1] = this->pRBody->GetPosition().y* meterToPixelRatio;
    game_data.playerL[0] = this->pLBody->GetPosition().x* meterToPixelRatio;
    game_data.playerL[1] = this->pLBody->GetPosition().y* meterToPixelRatio;

    if(this->game_data.gameState == GameStates::Playing) {
        if (game_data.ball[0] < config_data.goalAreaInPixel) {
            this->changeState(GameStates::PlayerLWon);
        } else if (game_data.ball[0] > config_data.fieldWithInPixel - config_data.goalAreaInPixel) {
            this->changeState(GameStates::PlayerRWon);
        }
    }

    gameDataBuffer->write(&game_data, sizeof(game_data), true);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

PoceGame::~PoceGame() {
    delete program;
    delete detector;
    delete texture;
    delete vertexBuffer;
    delete gameDataBuffer;
}

void PoceGame::changeState(GameStates state) {
    std::cout << "Changing state to: " << state << std::endl;
    this->game_data.gameState = state;
    this->game_data.stateAge = 0;
}

bool PoceGame::isWon() {
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
    players plrs = this->lastPlayers;
    plrs.right.valid = false;
    plrs.left.valid = false;
    std::vector<FaceDetector::Point> noses = detector->detectNoses(texture->getMat(), cv::Size(640, 480));
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
    return plrs;
}
