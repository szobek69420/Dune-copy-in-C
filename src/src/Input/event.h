#ifndef EVENT_H
#define EVENT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef enum {
    NONE = 0,
    WINDOW_RESIZE,
    KEY_PRESSED, KEY_RELEASED,
    MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED
} EventType;

typedef struct {
    int width, height;
} WindowResizeEvent;

typedef struct {
    int key_code;
    int mods;
} KeyPressedEvent;

typedef struct {
    int key_code;
} KeyReleasedEvent;

typedef struct {
    int button;
} MouseButtonPressedEvent;

typedef struct {
    int button;
} MouseButtonReleasedEvent;

typedef struct {
    double x, y;
} MouseMovedEvent;

typedef struct {
    double x, y;
} MouseScrolledEvent;

typedef struct {
    EventType type;
    union EventData
    {
        WindowResizeEvent window_resize;
        KeyPressedEvent key_pressed;
        KeyReleasedEvent key_released;
        MouseButtonPressedEvent mouse_button_pressed;
        MouseButtonReleasedEvent mouse_button_released;
        MouseMovedEvent mouse_moved;
        MouseScrolledEvent mouse_scrolled;
    } data;
} Event;

#endif