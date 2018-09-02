//
// Created by obyoxar on 02.09.18.
//

#include <stdexcept>
#include <map>
#include <iostream>
#include "Window.h"

std::map<GLFWwindow*, Window*> windowMapping;

void __key_callback_fun(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(windowMapping.find(window) != windowMapping.end()){
        windowMapping[window]->keyDown(key, scancode, action, mods);
    }else{
        throw std::runtime_error("Got keyEvent in unknown window!");
    }
}

Window::Window(std::string title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(640, 480, title.c_str(), nullptr, nullptr);
    windowMapping[window] = this;
    if(!window){
        throw std::runtime_error("Couldn't create Window");
    }
    glfwSetKeyCallback(window, __key_callback_fun);
}

Window::~Window() {
    windowMapping.erase(this->window);
    glfwDestroyWindow(this->window);
}

void Window::keyDown(int key, int scancode, int action, int mods) {
    std::cout << " Key: " << key << " Scancode: " <<
        scancode << " Action: " << action << " Mods: " << mods << std::endl;
}

void Window::makeCurrent() {
    glfwMakeContextCurrent(this->window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->window) == GLFW_TRUE;
}

cv::Size Window::getSize() const {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    return cv::Size(width, height);
}

void Window::swapBuffers() {
    glfwSwapBuffers(this->window);
}

void Window::requestClose() {
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}
