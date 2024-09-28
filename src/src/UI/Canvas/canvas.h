#ifndef CANVAS_H
#define CANVAS_H

//autoScale: if enabled, the canvas will stretch
void* canvas_create(const char* name, int autoScale);

void canvas_setAutoScale(void* element, int autoScale);

#endif