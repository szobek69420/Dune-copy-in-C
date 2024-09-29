#define _CRT_SECURE_NO_WARNINGS

#include "ui.h"

#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "../Renderer/Window/window.h"
#include "../Renderer/Fonts/fonts.h"
#include "Text/text.h"
#include "Image/image.h"
#include "Canvas/canvas.h"
#include "Button/button.h"

#include "../Input/input.h"

struct RootElement {
	UIComponent component;
};
typedef struct RootElement RootElement;


seqtor_of(void*) ELEMENTS;
RootElement* ROOT=NULL;


void ui_init()
{
	seqtor_init(ELEMENTS, 1);

	ROOT = malloc(sizeof(RootElement));
	ROOT->component = ui_initComponent("root");

	seqtor_push_back(ELEMENTS, ROOT);
}

void ui_deinit()
{
	if (ROOT != NULL)
		ui_destroyElement(ROOT);
	ROOT = NULL;

	while (seqtor_size(ELEMENTS) > 0)
		ui_destroyElement(seqtor_at(ELEMENTS, 0));

	seqtor_destroy(ELEMENTS);
}


void* ui_createElement(UIElementType type,const char* name)
{
	void* element = NULL;
	switch (type)
	{
	case UI_CANVAS:
		element= canvas_create(name,69);
		break;

	case UI_TEXT:
		element = text_create(name);
		break;

	case UI_IMAGE:
		element = image_create(name);
		break;

	case UI_BUTTON:
		element = button_create(name);
		break;
	}

	if (element != NULL)
		seqtor_push_back(ELEMENTS, element);
	return element;
}

void ui_destroyElement(void* element)
{
	RootElement* re = element;
	printf("delete: %s\n", re->component.name);
	while (seqtor_size(re->component.children) > 0)
	{
		ui_destroyElement(seqtor_at(re->component.children, 0));
		//no need for removal from the children list as the ui_destroyElement handles that as well
	}
	
	seqtor_remove(ELEMENTS, re);

	if (re->component.parent != NULL)//remove from parent
		seqtor_remove(((RootElement*)re->component.parent)->component.children, re);

	if (re->component.destroy != NULL)
		re->component.destroy(element);
	else
	{
		ui_destroyComponent(&re->component);
		free(re);
	}
}


void ui_addElement(void* element, void* parent)
{
	RootElement* e = element;
	RootElement* p = parent;

	if (e->component.parent != NULL)
		seqtor_remove(((RootElement*)e->component.parent)->component.children, e);

	if (p == NULL&&e!=ROOT)
		p = ROOT;
	seqtor_push_back(p->component.children, e);
	e->component.parent = p;
}

void ui_removeAll()
{
	while (seqtor_size(ROOT->component.children) > 0)
		ui_destroyElement(seqtor_at(ROOT->component.children, 0));
}

void ui_resizeHelper(void* element, int width, int height)
{
	RootElement* re = element;

	for (int i = 0; i < seqtor_size(re->component.children); i++)
		ui_resizeHelper(seqtor_at(re->component.children, i), width, height);

	if (re->component.onScreenResize != NULL)
		re->component.onScreenResize(re, width, height);
}
void ui_render()
{
	static int previousScreenWidth = -1;
	static int previousScreenHeight = -1;

	int screenWidth = window_width();
	int screenHeight = window_height();

	if (previousScreenWidth != screenWidth || previousScreenHeight != screenHeight)
	{
		previousScreenWidth = screenWidth;
		previousScreenHeight = screenHeight;

		fonts_setScreenSize(screenWidth, screenHeight);
		image_setScreenSize(screenWidth, screenHeight);

		ui_resizeHelper(ROOT, screenWidth, screenHeight);
	}

	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < seqtor_size(ROOT->component.children); i++)
	{
		RootElement* e = seqtor_at(ROOT->component.children, i);
		if (e->component.isVisible && e->component.render != NULL)
			e->component.render(e, 0, 0, screenWidth, screenHeight);
	}
}

#define IS_POINTER_IN_BOUNDS(X_INT,Y_INT) ((X_INT)>minX&&(X_INT)<maxX&&(Y_INT)>minY&&(Y_INT)<maxY)

void ui_handleInputHelper(
	struct { UIComponent component; } *element, 
	enum { PRESSED, HELD, RELEASED, NONE } state,
	int minX, int minY, int maxX, int maxY
)
{
	if (element->component.isInteractable == 0)
		return;

	ui_calculateBounds(element, &minX, &minY, &maxX, &maxY);

	double _x, _y;
	input_getMousePosition(&_x, &_y);
	int x = (int)_x;
	int y = (int)_y;

	if (!IS_POINTER_IN_BOUNDS(x, y))
		goto handle_children;

	switch (state)
	{
	case PRESSED:
		if (element->component.onPress != NULL)
			element->component.onPress(element);
		break;

	case RELEASED:
		if (element->component.onRelease != NULL)
			element->component.onRelease(element);
		break;

	case HELD:
		if (element->component.onHold != NULL)
			element->component.onHold(element);
		break;

	default:
		if (element->component.onHover != NULL)
			element->component.onHover(element);
		break;
	}

handle_children:

	for (int i = 0; i < seqtor_size(element->component.children); i++)
		ui_handleInputHelper(seqtor_at(element->component.children, i), state, minX, minY, maxX, maxY);
}

void ui_handleInput()
{
	enum { PRESSED, HELD, RELEASED, NONE } state = NONE;
	if (input_isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		state = PRESSED;
	else if (input_isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT))
		state = RELEASED;
	else if (input_isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
		state = HELD;

	for(int i=0;i<seqtor_size(ROOT->component.children);i++)
		ui_handleInputHelper(seqtor_at(ROOT->component.children,i), state, 0, 0, window_width(), window_height());
}

void ui_calculateBounds(void* element, int* minX, int* minY, int* maxX, int* maxY)
{
	RootElement* re = element;

	int currentX = 0;
	int currentY = 0;

	switch (re->component.hAlign)
	{
	case ALIGN_LEFT:
		currentX = *minX + re->component.xPos;
		break;

	case ALIGN_CENTER:
		currentX = (*minX + *maxX) / 2 + re->component.xPos - re->component.width / 2;
		break;

	case ALIGN_RIGHT:
		currentX = *maxX - re->component.xPos - re->component.width;
		break;
	}

	switch (re->component.vAlign)
	{
	case ALIGN_TOP:
		currentY = *minY + re->component.yPos;
		break;

	case ALIGN_CENTER:
		currentY = (*minY + *maxY) / 2 + re->component.yPos - re->component.height / 2;
		break;

	case ALIGN_BOTTOM:
		currentY = *maxY - re->component.yPos - re->component.height;
		break;
	}

	*minX = currentX;
	*minY = currentY;
	*maxX = currentX + re->component.width;
	*maxY = currentY + re->component.height;
}



UIComponent ui_initComponent(const char* name)
{
	UIComponent uic;

	uic.xPos = 0;
	uic.yPos = 0;
	uic.width = 10;
	uic.height = 10;
	uic.name = malloc((strlen(name) + 1) * sizeof(char));
	strcpy(uic.name, name);
	
	uic.hAlign = ALIGN_CENTER;
	uic.vAlign = ALIGN_CENTER;

	uic.isInteractable = 69;
	uic.isVisible = 69;

	seqtor_init(uic.children, 1);
	uic.parent = NULL;

	uic.render = NULL;
	uic.destroy = NULL;
	uic.onPress = NULL;
	uic.onHold = NULL;
	uic.onRelease = NULL;
	uic.onHover = NULL;
	uic.onScreenResize = NULL;

	return uic;
}

void ui_destroyComponent(UIComponent* uic)
{
	free(uic->name);
	seqtor_destroy(uic->children);
}