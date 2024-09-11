#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera/camera.h"
#include "Glm2/vec3.h"

GLFWwindow* init_window(const char* name, int width, int height);

void* mainCum;

int main()
{
    mainCum = camera_create((Vec3) { 0, 0, 0 }, (Vec3) { 0, 1, 0 }, 1);
    GLFWwindow* window = init_window("Strobogus", 600, 600);

    while (glfwWindowShouldClose(window) == 0)
    {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    camera_destroy(mainCum);
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