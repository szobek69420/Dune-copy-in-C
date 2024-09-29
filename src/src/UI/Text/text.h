#ifndef TEXT_H
#define TEXT_H

#include "../../Renderer/Fonts/fonts.h"

void* text_create(const char* name);

void text_setText(void* element, const char* text);
void text_setFontSize(void* element, int fontSize);
void text_setOrigin(void* element, TextOrigin hOrigin, TextOrigin vOrigin);
void text_setColour(void* element, float r, float g, float b, float a);

void text_getColour(void* element, float* r, float* g, float* b, float* a);

void text_pack(void* element);//calculates the minimal size so that the text is in the element

#endif