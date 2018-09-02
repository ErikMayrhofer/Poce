//
// Created by obyoxar on 02.09.18.
//

#ifndef DLIBTEST_APP_H
#define DLIBTEST_APP_H

#include <GL/glew.h>
#include <iostream>
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
    virtual void loop(){};
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

    {
        int glew = glewInit();
        if(glew != GLEW_OK){
            throw std::runtime_error("Couldn't init glew with code: " + std::to_string(glew));
        }
    }

    game = new T(this);

    game->init();

    while(!window->shouldClose()){
        game->loop();

        window->swapBuffers();
        glfwPollEvents();
    }
}


#endif //DLIBTEST_APP_H
