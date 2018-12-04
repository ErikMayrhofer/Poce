//
// Created by obyoxar on 02.09.18.
//

#ifndef DLIBTEST_APP_H
#define DLIBTEST_APP_H

#include <glad/glad.h>

#include <iostream>
#include <chrono>
#include "Window.h"

class Game;

class App {
    friend class Game;
public:
    App();
    ~App();

    template<typename T>
    void launch();

    Window *getWindow() const;

private:
    Window* window;
    Game* game;
};

class Game {
    friend class App;
public:
    explicit Game(App* app);
    virtual ~Game() = default;

    virtual void init(){};
    virtual void loop(double deltaMillis){};
    App* getApp();

private:
    App* app;

};

void error_callback(int , const char* description);

template<typename T>
void App::launch() {
    glfwSetErrorCallback(error_callback);
    {
        int glfw = glfwInit();
        if(glfw != GLFW_TRUE){
            throw std::runtime_error("Couldn't init glfw with code: " + std::to_string(glfw));
        }
    }

    window = new Window("App");
    window->makeCurrent();
    glfwSwapInterval(1);

    /*
    {
        GLenum glew = glewInit();
        if(glew != GLEW_OK){
            const char* error = reinterpret_cast<const char*>(glewGetErrorString(glew));
            std::string errorstr(error);
            throw std::runtime_error("Couldn't init glew with code: " + errorstr);
        }
    }
    */

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        throw std::runtime_error("GLAD failed!");
    }

    game = new T(this);

    game->init();


    auto lastTick = std::chrono::system_clock::now();
    while(!window->shouldClose()){
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> delta = (now-lastTick);
        lastTick = now;

        game->loop(delta.count());

        window->swapBuffers();
        glfwPollEvents();
    }
}


#endif //DLIBTEST_APP_H
