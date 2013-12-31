#include <GLFW/glfw3.h>

/* include some silly stuff */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "winfw.h"

#define UPDATE_INTERVAL 100000000       /* 10ms */

void font_init();

static GLFWwindow* window;
// Mouse position
static int xpos = 0, ypos = 0;

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
    // Remember cursor position
    xpos = (int)x;
    ypos = (int)y;
}

static void 
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS) {
        ejoy2d_win_touch(xpos, ypos, TOUCH_BEGIN);
    }
    else if (action == GLFW_RELEASE) {
        ejoy2d_win_touch(xpos,ypos,TOUCH_END);
    }
    else {
        ejoy2d_win_touch(xpos,ypos,TOUCH_MOVE);
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

    window = glfwCreateWindow(640, 480, "ejoy2d", NULL, NULL);
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

// attribute index
enum{
ATTRIB_VERTEX,
ATTRIB_COLOR,
NUM_ATTRIBUTES
};
int
main(int argc, char *argv[]) {
    uint32_t timestamp = 0;

    init_window();
    font_init();
    ejoy2d_win_init(argc, argv);

    while (!glfwWindowShouldClose(window)) {
        static const GLfloat squareVertices[] = {
                -0.5f, -0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f,
                -0.5f,  0.5f, 0.0f,
                 0.5f,  0.5f, 0.0f
        };

        static const GLubyte squareColors[] = {
                255, 255,   0, 255,
                0,   255, 255, 255,
                0,     0,   0,   0,
                255,   0, 255, 255,
        };
         // Update attribute values
        glClear(GL_COLOR_BUFFER_BIT);
        glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, squareVertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);
        glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, 0, squareColors);
        glEnableVertexAttribArray(ATTRIB_COLOR);

        glfwSwapBuffers(window);

        //float ratio;
        //int width, height;

        //glfwGetFramebufferSize(window, &width, &height);
        //ratio = width / (float) height;

        //glViewport(0, 0, width, height);

        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        //glMatrixMode(GL_MODELVIEW);

        //glLoadIdentity();
        //glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

        //glBegin(GL_TRIANGLES);
        //glColor3f(1.f, 0.f, 0.f);
        //glVertex3f(-0.6f, -0.4f, 0.f);
        //glColor3f(0.f, 1.f, 0.f);
        //glVertex3f(0.6f, -0.4f, 0.f);
        //glColor3f(0.f, 0.f, 1.f);
        //glVertex3f(0.f, 0.6f, 0.f);
        //glEnd();

        glfwPollEvents();
      //      ejoy2d_win_update();
      //      update_frame();

      //  uint32_t old = timestamp;
      //  timestamp= _gettime();
      //  if (timestamp - old >= UPDATE_INTERVAL) {
      //      ejoy2d_win_update();
      //      update_frame();
      //  }
      //  else {
      //      usleep(1000000000);
      //  }
    }
    return 0;
}

