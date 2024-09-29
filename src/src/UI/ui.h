#ifndef UI_H
#define UI_H

#include <seqtor.h>

enum AlignUI {
	ALIGN_LEFT, ALIGN_CENTER,ALIGN_RIGHT,ALIGN_TOP,ALIGN_BOTTOM
};
typedef enum AlignUI AlignUI;

struct UIComponent {
	char* name;
	int width, height;
	int xPos, yPos;
	AlignUI hAlign, vAlign;

	int isVisible, isInteractable;//these cascade

	seqtor_of(void*) children;
	void* parent;

	void (*render)(void*, int, int, int, int);//top left xy, bottom right xy
	void (*destroy)(void*);
	void (*onPress)(void*);
	void (*onHold)(void*);
	void (*onRelease)(void*);
	void (*onHover)(void*);
	void (*onScreenResize)(void*, int, int);//width,height
};

typedef struct UIComponent UIComponent;//every ui element has to have a "UIComponent" member variable as first


enum UIElementType {
	UI_CANVAS,
	UI_TEXT,
	UI_IMAGE,
	UI_BUTTON
};

typedef enum UIElementType UIElementType;


void ui_init();
void ui_deinit();


void* ui_createElement(UIElementType type,const char* name);
void ui_destroyElement(void* element);


void ui_addElement(void* element, void* parent);
void ui_removeAll();

void ui_render();
void ui_handleInput();

//the parameters serve both as input (parent's bounds) and output values
void ui_calculateBounds(void* element, int* minX, int* minY, int* maxX, int* maxY);


UIComponent ui_initComponent(const char* name);
void ui_destroyComponent(UIComponent* uic);

#endif