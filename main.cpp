#include <GL/glew.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <random>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include "OShader.h"
#include "OShaderProgram.h"
#include "VideoSource.h"
#include "Buffer.h"
#include "Texture.h"
#include "VideoTexture.h"
#include "FaceDetector.h"
#include "monitoring.h"

using namespace std;

#define INIT(type, expected) if(int a = type##Init() != (expected)){std::cerr << "Couldn't init " << #type << ": " << a << std::endl; exit(a);}
#define CHECK(object, msg, code) if(!(object)){std::cerr << (msg) << #object << std::endl; exit(code);}


#define CAMERA_COLOR_MODE GL_RGB
#define CAMERA_WIDTH 1920
#define CAMERA_HEIGHT 1280
#define CAMERA_GL_TYPE GL_UNSIGNED_BYTE
#define CAMERA_DATA_TYPE uchar

typedef CAMERA_DATA_TYPE camelement;

static const struct
{
    float x, y;
    float u, v;
} vertices[4] =
        {
                { -1.f, -1.f,  0.f,  0.f },
                { -1.f,  1.f,  0.f,  1.f },
                {  1.f, -1.f,  1.f,  0.f },
                {  1.f,  1.f,  1.f,  1.f }
        };

struct ubo_data_game_data{
    float playerL[2];
    float playerR[2];
    float ball[2];
};

bool detectFaces = false;

void error_callback(int , const char* description)
{
    std::cerr << "GLFW-Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int/*scancode*/, int action, int/* mods*/){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_F:
                detectFaces = !detectFaces;
                break;

        }
    }
}

GLFWwindow* init(){
    glfwSetErrorCallback(error_callback);
    INIT(glfw, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Poce", nullptr, nullptr);
    CHECK(window, "Couldn't open", 1)
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    INIT(glew, GLEW_OK);
    return window;
}

glm::mat4x4 calculateMVP(int width, int height){
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4x4 p, m, mvp;
    m = glm::mat4(1.0f);
    p = glm::ortho(-1.f, 1.f, 1.f, -1.f, 1.f, -1.f);
    mvp = p * m;
    return mvp;
}

int main( int argc, char **argv )
{
    GLFWwindow* window = init();


    OShaderProgram program("shaders/simple.v.glsl", "shaders/simple.f.glsl");
    GLuint mvp_location = program.getUniformLocation("mvp");
    GLuint vpos_location = program.getAttribLocation("position");
    GLuint tex_attrib = program.getAttribLocation("texcoord");
    program.use();

    FaceDetector detector("shape_predictor_68_face_landmarks.dat");

    VideoTexture texture(CAMERA_WIDTH, CAMERA_HEIGHT);
    texture.setUniformToUnit(program.getUniformLocation("texture_sampler"));

    Buffer<float> vertexBuffer(BufferType::Array, StaticDraw);
    vertexBuffer.write(vertices, sizeof(vertices), true);

    ubo_data_game_data game_data {};
    Buffer<float> gameDataBuffer(BufferType::Uniform, DynamicDraw);
    gameDataBuffer.write(&game_data, sizeof(game_data), true);
    program.bindUBOTo(gameDataBuffer, "game_data", 1);


    withBuffer(vertexBuffer, {
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, vertexBuffer.getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 0));

        glEnableVertexAttribArray(tex_attrib);
        glVertexAttribPointer(tex_attrib, 2, vertexBuffer.getGLDataType(), GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 2));
    })


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    int a = 0;



    while(!glfwWindowShouldClose(window)){

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(calculateMVP(width, height)));

        texture.update();

        if(detectFaces){
            std::cout << "Detecting" << std::endl;
            std::vector<FaceDetector::Point> noses = detector.detectNoses(texture.getMat(), cv::Size(640, 480));
            if(!noses.empty()){
                game_data.playerR[0] = static_cast<float>(noses[0].x / 640.0);
                game_data.playerR[1] = static_cast<float>(noses[0].y / 480.0);
            }
            if(noses.size() > 1){
                game_data.playerL[0] = static_cast<float>(noses[1].x / 640.0);
                game_data.playerL[1] = static_cast<float>(noses[1].y / 480.0);
            }
        }
        a -= 5;
        if(a < -width) a = width;
        float pct = (float) abs(a) / (float) width;
        game_data.ball[0] = pct;
        std::cout << pct << std::endl;
        gameDataBuffer.write(&game_data, sizeof(game_data), true);


        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}