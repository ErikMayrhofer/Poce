
#include <glad/glad.h>
#include <FTGL/ftgl.h>
#include <iostream>

#include <GLFW/glfw3.h>
#include <chrono>
#include "Window.h"

int main(int argc, char* argv[]){
    {
        int glfw = glfwInit();
        if(glfw != GLFW_TRUE){
            throw std::runtime_error("Couldn't init glfw with code: " + std::to_string(glfw));
        }
    }

    auto window = new Window("App");
    window->makeCurrent();
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        throw std::runtime_error("GLAD failed!");
    }

    FTGLPixmapFont font("fonts/arial.ttf");

    auto error = font.Error();

    if(error){
        std::cout << "Font-Error: " << error << std::endl;
    }

    font.FaceSize(72);
    font.Render("Hello World");



    auto lastTick = std::chrono::system_clock::now();
    int a = 0;
    while(!window->shouldClose()){
        glClear(GL_COLOR_BUFFER_BIT);

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> delta = (now-lastTick);
        lastTick = now;

        //LOOP

        std::stringstream s;
        s << "Hello World" << (a++);
        font.Render(s.str().c_str());


        window->swapBuffers();
        glfwPollEvents();
    }
    return 0;
}