//
// Created by obyoxar on 02.09.18.
//

#include "App.h"

App *Game::getApp() {
    return this->app;
}

Game::Game(App *app) : app(app){

}

void error_callback(int , const char* description)
{
    std::cerr << "GLFW-Error: " << description << std::endl;
}

App::App() = default;

App::~App() {
    delete window;
    delete game;
    glfwTerminate();
}

Window *App::getWindow() const {
    return window;
}
