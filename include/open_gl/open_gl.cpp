#include "open_gl.h"

OpenGL::OpenGL(Canvas* canvas, float pixelScale, ScreenMode screenMode, const char* title)
{
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (screenMode == FULLSCREEN) {
        GLFWmonitor* primaryMonitor =  glfwGetPrimaryMonitor();
        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
        window = glfwCreateWindow(videoMode->width, videoMode->height, title, primaryMonitor, NULL);

        (*canvas).init(
            videoMode->width / pixelScale,
            videoMode->height / pixelScale
        );

        for (int i = 0; i < 24; i += 4) {
            vertices[i + 0] *= (float)canvas->width / videoMode->width * pixelScale;
            vertices[i + 1] *= (float)canvas->height / videoMode->height * pixelScale;
        }
    } else {
        glfwWindowHint(GLFW_RESIZABLE, (screenMode == WINDOWED_RESIZABLE ? GLFW_TRUE : GLFW_FALSE));
        glfwWindowHint(GLFW_DECORATED, (screenMode != WINDOWED_BORDERLESS ? GLFW_TRUE : GLFW_FALSE));
        window = glfwCreateWindow(canvas->width * pixelScale, canvas->height * pixelScale, title, NULL, NULL);
    }

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    initQuad(canvas);

    shader = new Shader(
        "include/open_gl/shaders/canvas.vert",
        "include/open_gl/shaders/canvas.frag"
    );

    currTime = 0;
    deltaTime = 0;
    lastTime = static_cast<float>(glfwGetTime());
}

OpenGL::~OpenGL()
{
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteFramebuffers(1, &frame_buffer);
    glDeleteTextures(1, &texture_color_buffer);
    glDeleteRenderbuffers(1, &render_buffer);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void OpenGL::update(Canvas* canvas)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(.1f, .2f, .2f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    glBindVertexArray(vertex_array);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas->width, canvas->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas->pixels);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();

    currTime = static_cast<float>(glfwGetTime());
    deltaTime = currTime - lastTime;
    lastTime = currTime;
}

void OpenGL::initQuad(Canvas* canvas)
{
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glGenTextures(1, &texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas->width, canvas->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

    glGenRenderbuffers(1, &render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, canvas->width, canvas->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
