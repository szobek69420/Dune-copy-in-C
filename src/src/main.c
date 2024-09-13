#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameObjects/game_object.h"
#include "Renderer/renderer.h"

#include "Camera/camera.h"
#include "Glm2/vec3.h"

GLFWwindow* init_window(const char* name, int width, int height);

//externables
void* MAIN_CUM;

int main()
{
    GLFWwindow* window = init_window("Strobogus", 600, 600);

    MAIN_CUM = camera_create((Vec3) { 0, 0, 3 }, (Vec3) { 0, 1, 0 });
    camera_setProjection(MAIN_CUM, 10, 5, -10, 10);
    camera_setForward(MAIN_CUM, (Vec3) { 0, 0, -1 });
    renderer_init();
    renderer_setCamera(MAIN_CUM);
    gameObject_init();

    double frameStart = 0;
    while (glfwWindowShouldClose(window) == 0)
    {
        double deltaTime = glfwGetTime() - frameStart;
        frameStart += deltaTime;
        if (deltaTime > 0.5f)
        {
            printf("Update skipped\n");
            continue;
        }

        gameObject_update((float)deltaTime);

        glClearColor(1, 0, 0, 1);
        glClearDepth(1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        gameObject_render(window);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    gameObject_deinit();
    renderer_deinit();
    camera_destroy(MAIN_CUM);
    return 0;
}


GLFWwindow* init_window(const char* name, int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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