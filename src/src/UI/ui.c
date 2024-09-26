#define _CRT_SECURE_NO_WARNINGS

#include "ui.h"

#include <stdlib.h>
#include <string.h>

#include "../Renderer/Window/window.h"

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
	if(ROOT!=NULL)
		ui_destroyElement(ROOT);
	ROOT = NULL;

	while (seqtor_size(ELEMENTS) > 0)
		ui_destroyElement(seqtor_at(ELEMENTS, 0));

	seqtor_destroy(ELEMENTS);
}


void* ui_createElement(UIElementType type)
{
	switch (type)
	{
	case UI_CANVAS:

		break;

	case UI_TEXT:

		break;

	case UI_IMAGE:

		break;

	case UI_BUTTON:

		break;
	}
}

void ui_destroyElement(void* element)
{
	seqtor_remove(ELEMENTS, element);

	if (((RootElement*)element)->component.destroy != NULL)
		((RootElement*)element)->component.destroy(element);
	else
		free(element);
}


void ui_addElement(void* element, void* parent)
{
	RootElement* e = element;
	RootElement* p = parent;

	if (e->component.parent != NULL)
		seqtor_remove(((RootElement*)e->component.parent)->component.children, e);

	seqtor_push_back(p->component.children, e);
	e->component.parent = parent;
}

void ui_render()
{
	int screenWidth = window_width();
	int screenHeight = window_height();

	for (int i = 0; i < seqtor_size(ROOT->component.children); i++)
	{
		RootElement* e = seqtor_at(ROOT->component.children, i);
		if (e->component.isVisible && e->component.render != NULL)
			e->component.render(e, 0, 0, screenWidth, screenHeight);
	}
}



UIComponent ui_initComponent(const char* name)
{
	UIComponent uic;

	uic.xPos = 0;
	uic.yPos = 0;
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
}

void ui_destroyComponent(UIComponent* uic)
{
	free(uic->name);
	seqtor_destroy(uic->children);
}