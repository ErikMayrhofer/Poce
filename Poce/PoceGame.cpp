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

    std::cout << widthInM << "x" << heightInM << std::endl;

    //World10
    this->world = new b2World(b2Vec2(0.0f, 9.81f));

    //Ground
    b2BodyDef* groundBodyDef = new b2BodyDef();
    b2PolygonShape* groundBodyShape = new b2PolygonShape();
    groundBodyDef->position.Set(widthInM/2.f, heightInM+5.f);
    groundBodyShape->SetAsBox(widthInM/2, 10.f);
    this->groundBody = this->world->CreateBody(groundBodyDef);
    this->groundBody->CreateFixture(groundBodyShape, 0.0f);


    //Top
    b2BodyDef* topBodyDef = new b2BodyDef();
    b2PolygonShape* topBodyShape = new b2PolygonShape();
    topBodyDef->position.Set(widthInM/2.f, -5.f);
    topBodyShape->SetAsBox(widthInM/2, 10.f);
    this->topBody = this->world->CreateBody(topBodyDef);
    this->topBody->CreateFixture(topBodyShape, 0.0f);

    b2BodyDef* ballDef = new b2BodyDef();
    ballDef->type = b2_dynamicBody;
    ballDef->position.Set(widthInM/2, heightInM/2);
    ballDef->linearDamping = 10.0f;
    //b2PolygonShape* ballShape = new b2PolygonShape();
    //ballShape->SetAsBox(0.15f, 0.15f);
    b2CircleShape* ballShape = new b2CircleShape();
    ballShape->m_radius = 0.15f;
    b2FixtureDef* ballFixDef = new b2FixtureDef;
    ballFixDef->shape = ballShape;
    ballFixDef->density = 20.f;
    ballFixDef->friction = 0.3f;
    this->ballBody->CreateFixture(ballFixDef);
    this->ballBody = this->world->CreateBody(ballDef);
}

void PoceGame::loop(double deltaMS) {
    cv::Size size = getApp()->getWindow()->getSize();

    //std::cout << 1/(deltaMS/1000) << std::endl;



    this->world->Step(static_cast<float32>(deltaMS / 1000), 6, 2);




    texture->update();
    float wWidth = size.width;
    float wHeight = size.height;
    float wAspect = wWidth/wHeight; //a = w/h ----- w = a * h ----- h = w / a
    float mWidth = texture->getMat().cols;
    float mHeight = texture->getMat().rows;
    float mAspect = mWidth / mHeight;
    float cWidth = 1000;
    float cHeight = cWidth / mAspect;

    float meterToPixelRatio = cWidth / config_data.fieldWidthInM; //meter * r = pixel


    vertices[1].y = cHeight;
    vertices[2].x = cWidth;
    vertices[3].x = cWidth;
    vertices[3].y = cHeight;
    vertexBuffer->write(vertices, sizeof(vertices), true);
    game_data.fieldSize[0] = cWidth;
    game_data.fieldSize[1] = cHeight;
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

    std::vector<FaceDetector::Point> noses = detector->detectNoses(texture->getMat(), cv::Size(640, 480));
    if(!noses.empty()){
        game_data.playerR[0] = static_cast<float>(noses[0].x) * cWidth;
        game_data.playerR[1] = static_cast<float>(noses[0].y) * cHeight;
    }
    if(noses.size() > 1){
        game_data.playerL[0] = static_cast<float>(noses[1].x) * cWidth;
        game_data.playerL[1] = static_cast<float>(noses[1].y) * cHeight;
    }


    game_data.ball[0] = this->ballBody->GetPosition().x * meterToPixelRatio;
    game_data.ball[1] = this->ballBody->GetPosition().y * meterToPixelRatio;

    std::cout << game_data.ball[0] << " " << game_data.ball[1] << std::endl;
    std::cout << "--" << this->groundBody->GetPosition().x << " " << this->groundBody->GetPosition().y << std::endl;

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