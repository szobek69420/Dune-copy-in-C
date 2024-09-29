#include "game_loop.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "../GameObjects/game_object.h"
#include "../Renderer/renderer.h"
#include "../Renderer/Window/window.h"
#include "../Physics/physics.h"

#include "../Camera/camera.h"
#include "../Glm2/vec3.h"

#include "../Input/event.h"
#include "../Input/event_queue.h"
#include "../Input/input.h"

#include "../UI/ui.h"

GLFWwindow* init_window(const char* name, int width, int height);
void windowSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void gameLoop_setCurrentStage(GameStage stage);
void gameLoop_handleStageChange();

//externables
void* MAIN_CUM=NULL;

//state
static GLFWwindow* CURRENT_WINDOW = NULL;
static GameStage CURRENT_STAGE=GS_INIT;

void gameLoop_loop()
{
	while (69)
	{
        GameStage previousGameStage = CURRENT_STAGE;//it is deliberately before handleStageChange as the GS_INIT immediately changes the stage
        gameLoop_handleStageChange();

        if (CURRENT_STAGE == GS_DEINIT)
            break;

        int frameCount = 0;
        double elapsedTime = 0;
        double frameStart = 0;
        while (previousGameStage==CURRENT_STAGE)
        {
            double deltaTime = glfwGetTime() - frameStart;
            frameStart += deltaTime;
            if (deltaTime > 0.1f)
            {
                printf("Update skipped\n");
                continue;
            }

            elapsedTime += deltaTime;
            frameCount++;

            physics_step((float)deltaTime);


            input_update();
            Event e;
            while ((e = eventQueue_poll()).type != NONE)
                input_handleEvent(e);
            ui_handleInput();

            gameObject_update((float)deltaTime);

            glClearColor(0, 1, 1, 1);
            glClearDepth(1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gameObject_render(CURRENT_WINDOW);
            ui_render();

            glfwSwapBuffers(CURRENT_WINDOW);
            glfwPollEvents();

            if (glfwWindowShouldClose(CURRENT_WINDOW) != 0)
                gameLoop_setCurrentStage(GS_DEINIT);
        }

        printf("Average FPS: %.1f\n", frameCount / elapsedTime);
	}
}

void gameLoop_setCurrentStage(GameStage stage)
{
    CURRENT_STAGE = stage;
}

void gameLoop_handleStageChange()
{
    switch (CURRENT_STAGE)
    {
    case GS_INIT:
        do {
            CURRENT_WINDOW = init_window("Strobogus", 600, 600);
            glfwSetWindowSizeCallback(CURRENT_WINDOW, windowSizeCallback);
            glfwSetKeyCallback(CURRENT_WINDOW, keyCallback);
            glfwSetMouseButtonCallback(CURRENT_WINDOW, mouseButtonCallback);
            glfwSetCursorPosCallback(CURRENT_WINDOW, cursorPositionCallback);
            glfwSetScrollCallback(CURRENT_WINDOW, scrollCallback);

            windowSizeCallback(NULL, 600, 600);

            MAIN_CUM = camera_create((Vec3) { 0, 0, 5 }, (Vec3) { 0, 1, 0 });
            camera_setProjection(MAIN_CUM, 10, 10, 0, 10);
            camera_setForward(MAIN_CUM, (Vec3) { 0, 0, -1 });
            renderer_init();
            physics_init();

            ui_init();

            renderer_setCamera(MAIN_CUM);
            gameObject_init();

            input_init();
            eventQueue_init();

            //glEnable(GL_MULTISAMPLE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            gameLoop_setCurrentStage(GS_INGAME);
        } while (0);
        break;

    case GS_DEINIT:
        do {
            gameObject_deinit();
            ui_deinit();
            physics_deinit();
            renderer_deinit();
            camera_destroy(MAIN_CUM);

            MAIN_CUM = NULL;
            CURRENT_WINDOW = NULL;

            glfwTerminate();
        } while (0);
        break;

    case GS_INGAME:
        do {
            gameObject_removeAll();
            ui_removeAll();

            void* player = NULL;
            player = gameObject_create(PLAYER, "player");
            gameObject_add(player, NULL);

            void* trackHandler = NULL;
            trackHandler = gameObject_create(TRACK_HANDLER, "track_handler");
            gameObject_add(trackHandler, NULL);

            struct { UIComponent c; }*button = ui_createElement(UI_BUTTON, "glsadf");
            button->c.width = 200;
            ui_addElement(button, NULL);
        } while (0);
        break;
    }
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