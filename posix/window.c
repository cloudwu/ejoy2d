#include <GLFW/glfw3.h>

/* include some silly stuff */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "winfw.h"

#define UPDATE_INTERVAL 1       /* 10ms */
#define MOUSE_PRESS     1
#define MOUSE_RELEASE   0

void font_init();

static GLFWwindow* window;
static int mouse_press = MOUSE_RELEASE;

static uint32_t
_gettime(void) {
    uint32_t t;
#if !defined(__APPLE__)
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    t = (uint32_t)(ti.tv_sec & 0xffffff) * 100;
    t += ti.tv_nsec / 10000000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (uint32_t)(tv.tv_sec & 0xffffff) * 100;
    t += tv.tv_usec / 10000;
#endif

    return t;
}

static void
update_frame() {
    ejoy2d_win_frame();
    glfwSwapBuffers(window);
}

static void
error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void
cursor_position_callback(GLFWwindow* window, double x, double y)
{
    if (mouse_press==MOUSE_PRESS) {
        ejoy2d_win_touch((int)x,(int)y,TOUCH_MOVE);
    }
}

static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    double xpos = 0 ;
    double ypos = 0;
    glfwGetCursorPos(window,&xpos,&ypos);
    int x = (int)xpos;
    int y = (int)ypos;
    if (action == GLFW_PRESS) {
        ejoy2d_win_touch(x, y, TOUCH_BEGIN);
        mouse_press = MOUSE_PRESS;
    }
    else if (action == GLFW_RELEASE) {
        ejoy2d_win_touch(x,y,TOUCH_END);
        mouse_press = MOUSE_RELEASE;
    }
}

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
}

static void
init_window()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
       exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "ejoy2d", NULL, NULL);
    if (!window)
    {
       glfwTerminate();
       exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

int
main(int argc, char *argv[]) {
    uint32_t timestamp = 0;

    init_window();
    font_init();
    ejoy2d_win_init(argc, argv);

    const GLubyte* name = glGetString(GL_VENDOR);    
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* OpenGLVersion =glGetString(GL_VERSION); 
    printf("OpenGL Hardware Vendor : %s\n", name);
    printf("Renderer identifier    : %s\n", renderer);
    printf("OpenGL Version         : %s\n", OpenGLVersion);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        uint32_t old = timestamp;
        timestamp= _gettime();
        if (timestamp - old >= UPDATE_INTERVAL) {
            ejoy2d_win_update();
            update_frame();
        }
        else {
            usleep(1000);
        }
    }
    return 0;
}

