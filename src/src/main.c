#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameObjects/game_object.h"
#include "Renderer/renderer.h"
#include "Renderer/Window/window.h"
#include "Physics/physics.h"

#include "Camera/camera.h"
#include "Glm2/vec3.h"

#include "Input/event.h"
#include "Input/event_queue.h"
#include "Input/input.h"


#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

GLFWwindow* init_window(const char* name, int width, int height);
void windowSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

//externables
void* MAIN_CUM;

int main()
{
    //_CrtSetBreakAlloc(443);

    GLFWwindow* window = init_window("Strobogus", 600, 600);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    windowSizeCallback(NULL, 600, 600);

    MAIN_CUM = camera_create((Vec3) { 0, 0, 5 }, (Vec3) { 0, 1, 0 });
    camera_setProjection(MAIN_CUM, 10, 10, 0, 10);
    camera_setForward(MAIN_CUM, (Vec3) { 0, 0, -1 });
    renderer_init();
    physics_init();

    renderer_setCamera(MAIN_CUM);
    gameObject_init();

    input_init();
    eventQueue_init();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    double frameStart = 0;
    while (glfwWindowShouldClose(window) == 0&&input_isKeyPressed(GLFW_KEY_ESCAPE)==0)
    {
        double deltaTime = glfwGetTime() - frameStart;
        frameStart += deltaTime;
        if (deltaTime > 0.5f)
        {
            printf("Update skipped\n");
            continue;
        }

        physics_step((float)deltaTime);


        input_update();
        Event e;
        while ((e = eventQueue_poll()).type != NONE)
            input_handleEvent(e);

        gameObject_update((float)deltaTime);

        glClearColor(0, 1, 1, 1);
        glClearDepth(1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        gameObject_render(window);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    gameObject_deinit();
    physics_deinit();
    renderer_deinit();
    camera_destroy(MAIN_CUM);

#ifdef _CRTDBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}


GLFWwindow* init_window(const char* name, int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);//multisample

    GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL)//check if glfw is kaputt
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    //check if glad is kaputt
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return NULL;
    }

    glViewport(0, 0, width, height);

    return window;
}



//callbacks
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    window_set(width, height);
    glViewport(0, 0, width, height);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        eventQueue_push((Event) { .type = KEY_PRESSED, .data.key_pressed = { key, mods } });
    else if (action == GLFW_RELEASE)
        eventQueue_push((Event) { .type = KEY_RELEASED, .data.key_released = { key } });
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
        eventQueue_push((Event) { .type = MOUSE_BUTTON_PRESSED, .data.mouse_button_pressed = { button } });
    else if (action == GLFW_RELEASE)
        eventQueue_push((Event) { .type = MOUSE_BUTTON_RELEASED, .data.mouse_button_released = { button } });
}
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    eventQueue_push((Event) { .type = MOUSE_MOVED, .data.mouse_moved = { xpos, ypos } });
}
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    eventQueue_push((Event) { .type = MOUSE_SCROLLED, .data.mouse_scrolled = { xoffset, yoffset } });
}
