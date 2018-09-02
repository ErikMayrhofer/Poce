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

    ubo_data_game_data game_data {};
    gameDataBuffer = new Buffer<float>(BufferType::Uniform, DynamicDraw);
    gameDataBuffer->write(&game_data, sizeof(game_data), true);
    program->bindUBOTo(*gameDataBuffer, "game_data", 1);


    withBuffer(*vertexBuffer, {
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, vertexBuffer->getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 0));

        glEnableVertexAttribArray(tex_attrib);
        glVertexAttribPointer(tex_attrib, 2, vertexBuffer->getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 2));
    })
}

void PoceGame::loop() {
    cv::Size size = getApp()->getWindow()->getSize();

    texture->update();
    float wWidth = size.width;
    float wHeight = size.height;
    float wAspect = wWidth/wHeight; //a = w/h ----- w = a * h ----- h = w / a
    float mWidth = texture->getMat().cols;
    float mHeight = texture->getMat().rows;
    float mAspect = mWidth / mHeight;
    float cWidth = 1000;
    float cHeight = cWidth / mAspect;

    vertices[1].y = cHeight;
    vertices[2].x = cWidth;
    vertices[3].x = cWidth;
    vertices[3].y = cHeight;
    vertexBuffer->write(vertices, sizeof(vertices), true);

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