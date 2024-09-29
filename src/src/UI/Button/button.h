#ifndef BUTTON_H
#define BUTTON_H

void* button_create(const char* name);

void* button_getBackgroundImage(void* element);
void* button_getText(void* element);

void button_setNormalColour(void* element, float r, float g, float b, float a);
void button_setHoverColour(void* element, float r, float g, float b, float a);
void button_setPressedColour(void* element, float r, float g, float b, float a);

void button_setOnPress(void* element, void(*onPress)(void*));
void button_setOnHold(void* element, void(*onHold)(void*));
void button_setOnRelease(void* element, void(*onRelease)(void*));
void button_setOnHover(void* element, void(*onHover)(void*));

#endif