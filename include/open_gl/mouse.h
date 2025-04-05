#pragma once

#include "open_gl.h"

enum MouseMode {
    MOUSE_CURSOR_ENABLED,
    MOUSE_CURSOR_DISABLED,
    MOUSE_CURSOR_HIDDEN
};

class Mouse
{
    public:
        double mouseSensivity;

        double xPos, yPos, xPos_last, yPos_last, xDelta, yDelta;

        bool scrolled;
        double xScroll, yScroll;

        Mouse(OpenGL* openGL, double mouseSensivity = 1, MouseMode mouseMode = MOUSE_CURSOR_ENABLED)
        {
            this->mouseSensivity = mouseSensivity;

            switch (mouseMode) {
                case MOUSE_CURSOR_HIDDEN:
                    glfwSetInputMode(openGL->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                    glfwSetInputMode(openGL->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                    break;
                case MOUSE_CURSOR_DISABLED:
                    glfwSetInputMode(openGL->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                    glfwSetInputMode(openGL->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    break;
            }

            glfwGetCursorPos(openGL->window, &xPos, &yPos);
            xPos_last = xPos;
            yPos_last = yPos;

            glfwSetWindowUserPointer(openGL->window, this);
            glfwSetScrollCallback(openGL->window,
                [](GLFWwindow* window, double xOffset, double yOffset) {
                    Mouse* mouse = (Mouse*)glfwGetWindowUserPointer(window);

                    mouse->scrolled = true;
                    mouse->xScroll += xOffset;
                    mouse->yScroll += yOffset;
                }
            );

            scrolled = false;
            xScroll = 0;
            yScroll = 0;
        }

        void update(OpenGL* openGL)
        {
            glfwGetCursorPos(openGL->window, &xPos, &yPos);
            xDelta = xPos - xPos_last;
            yDelta = yPos - yPos_last;
            xPos_last = xPos;
            yPos_last = yPos;

            if (scrolled) {
                scrolled = false;
            } else {
                xScroll = 0;
                yScroll = 0;
            }
        }
};
