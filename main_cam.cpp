#include <GL/glew.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "OShader.h"
#include "OShaderProgram.h"
#include <opencv2/opencv.hpp>

#define INIT(type, expected) if(int a = type##Init() != expected){std::cerr << "Couldn't init " << #type << ": " << a << std::endl; return a;}
#define CHECK(object, msg, code) if(!object){std::cerr << msg << #object << std::endl; return code;}

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

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW-Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

GLuint loadShader(const char* file, GLenum type){
    std::cout << "Loading shader " << file << std::endl;

    std::ifstream ifs(file);
    if(!ifs.good()){
        std::cerr << "Error" << std::endl;
        std::cerr << ifs.fail() << std::endl;
    }
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );
    const char* txt = content.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &txt, NULL);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE){
        std::cerr << "Failed!" << std::endl;
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        GLsizei realLength;
        char log[logSize];

        glGetShaderInfoLog(shader, logSize, &realLength, log);

        std::cerr << log << std::endl;

        glDeleteShader(shader);
        return 0;
    }
    std::cout << "Success" << std::endl;
    return shader;
}

int main( int argc, char **argv )
{
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

    GLuint vertex_buffer, pixel_buffer;
    GLuint mvp_location, vpos_location, vcol_location, tex_attrib, sampler_loc;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    OShaderProgram program("shaders/simple.v.glsl", "shaders/simple.f.glsl");

    mvp_location = program.getUniformLocation("mvp");
    vpos_location = program.getAttribLocation("position");
    tex_attrib = program.getAttribLocation("texcoord");
    sampler_loc = program.getUniformLocation("texture_sampler");
    //vcol_location = program.getAttribLocation("vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 0));
    glEnableVertexAttribArray(tex_attrib);
    glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*) (sizeof(float) * 2));
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*) (sizeof(float) * 2));

    cv::VideoCapture cap("vid.mp4");
    if(!cap.isOpened())
        return -1;



    cv::Mat frame;
    cap >> frame;
    std::cout << (frame.type());
    void* data = frame.data;
    size_t fsize = frame.total() * frame.elemSize();

    glGenBuffers(1, &pixel_buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, fsize, data, GL_DYNAMIC_DRAW);


    GLuint tex;
    glGenTextures(1, &tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.size[0], frame.size[1], 0, GL_RGB, GL_UNSIGNED_INT, (void*) 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glUniform1i(sampler_loc, 0);

    while(!glfwWindowShouldClose(window)){
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        program.use();
        glm::mat4x4 p, m, mvp;
        m = glm::mat4(1.0f);
        //m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
        p = glm::ortho(-1.f, 1.f, 1.f, -1.f, 1.f, -1.f);
        mvp = p * m;
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));


        cv::Mat lframe;
        cap >> lframe;
        data = lframe.data;
        fsize = frame.total() * frame.elemSize();
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
        auto * pbomap = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        memcpy(pbomap, data, fsize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lframe.size[0], lframe.size[1], 0, GL_RGB, GL_UNSIGNED_INT, (void*) 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}