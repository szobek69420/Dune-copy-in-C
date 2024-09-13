#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "event.h"

typedef struct {
    int previous_key_state[GLFW_KEY_LAST];
    int current_key_state[GLFW_KEY_LAST];
    int key_modifiers[GLFW_KEY_LAST];
    int previous_mouse_button_state[GLFW_MOUSE_BUTTON_LAST];
    int current_mouse_button_state[GLFW_MOUSE_BUTTON_LAST];
    double previous_mouse_x, previous_mouse_y;
    double current_mouse_x, current_mouse_y;
    double mouse_scroll_delta_x, mouse_scroll_delta_y;
} InputData;

void input_init();
void input_update();
void input_handleEvent(Event e);

int input_isKeyPressed(int key);
int input_isKeyHeld(int key);
int input_isKeyReleased(int key);
int input_getKeyModifiers(int key);
int input_isKeyCapital(int key);

int input_isMouseButtonPressed(int button);
int input_isMouseButtonHeld(int button);
int input_isMouseButtonReleased(int button);

void input_getMousePosition(double* x, double* y);
void input_getMouseDelta(double* x, double* y);
void input_getMouseScrollDelta(double* x, double* y);

#endif