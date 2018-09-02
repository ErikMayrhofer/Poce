//
// Created by obyoxar on 02.09.18.
//

#ifndef DLIBTEST_POCE_WINDOW_H
#define DLIBTEST_POCE_WINDOW_H


#include <string>
#include <GLFW/glfw3.h>
#include <opencv2/core/types.hpp>
#include <boost/signals2/signal.hpp>

class Window {
public:
    explicit Window(std::string title);
    ~Window();

    void keyDown(int key, int scancode, int action, int mods);
    void makeCurrent();
    void swapBuffers();

    bool shouldClose() const;
    void requestClose();
    cv::Size getSize() const;

    boost::signals2::signal<void(int key, int scancode, int action, int mods)> onKeyDown;
private:
    GLFWwindow* window;
};


#endif //DLIBTEST_POCE_WINDOW_H
