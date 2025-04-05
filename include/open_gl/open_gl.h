#pragma once

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "canvas.h"

enum ScreenMode {
    FULLSCREEN,
    WINDOWED,
    WINDOWED_RESIZABLE,
    WINDOWED_BORDERLESS
};

class OpenGL
{
    public:
        GLFWwindow* window;
        float deltaTime;

        OpenGL(Canvas* canvas, float pixelScale = 1.0f, ScreenMode screenMode = WINDOWED, const char* title = "OpenGL - 2D window");
        ~OpenGL();

        void update(Canvas* canvas);

    private:
        float vertices[24] =  {
            // positions    // texCoords
            -1.0f,  1.0f,   0.0f, 1.0f,     // left top
            -1.0f, -1.0f,   0.0f, 0.0f,     // left bottom
             1.0f, -1.0f,   1.0f, 0.0f,     // right bottom

            -1.0f,  1.0f,   0.0f, 1.0f,     // left top
             1.0f, -1.0f,   1.0f, 0.0f,     // right bottom
             1.0f,  1.0f,   1.0f, 1.0f      // right top
        };

        GLuint vertex_array;
        GLuint vertex_buffer;
        GLuint frame_buffer;
        GLuint texture_color_buffer;
        GLuint render_buffer;

        Shader *shader;

        float currTime, lastTime;

        void initQuad(Canvas* canvas);
};
