#define _CRT_SECURE_NO_WARNINGS

#include "ui.h"

#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "../Renderer/Window/window.h"
#include "Text/text.h"
#include "Image/image.h"
#include "Canvas/canvas.h"
#include "../Renderer/Fonts/fonts.h"

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

	void* alma = ui_createElement(UI_CANVAS, "amogus");
	ui_addElement(alma, NULL);

	RootElement* alma2 = ui_createElement(UI_TEXT, "amogus2");
	ui_addElement(alma2, alma);
	text_setFontSize(alma2, 20);
	text_setText(alma2, "globus");
	text_setColour(alma2, 1, 0.85f, 0, 1);
	alma2->component.xPos = 0;
	alma2->component.yPos = 0;
	alma2->component.hAlign = ALIGN_RIGHT;
	alma2->component.vAlign = ALIGN_BOTTOM;
	text_pack(alma2);
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


void* ui_createElement(UIElementType type,const char* name)
{
	switch (type)
	{
	case UI_CANVAS:
		return canvas_create(name,69);

	case UI_TEXT:
		return text_create(name);

	case UI_IMAGE:
		return image_create(name);

	case UI_BUTTON:

		break;
	}
}

void ui_destroyElement(void* element)
{
	RootElement* re = element;

	while (seqtor_size(re->component.children) > 0)
	{
		ui_destroyElement(seqtor_at(re->component.children, 0));
		seqtor_remove_at(re->component.children, 0);
	}
	
	seqtor_remove(ELEMENTS, re);

	if (re->component.destroy != NULL)
		re->component.destroy(element);
	else
		free(element);
}


void ui_addElement(void* element, void* parent)
{
	RootElement* e = element;
	RootElement* p = parent;

	if (e->component.parent != NULL)
		seqtor_remove(((RootElement*)e->component.parent)->component.children, e);

	if (p == NULL)
		p = ROOT;
	seqtor_push_back(p->component.children, e);
	e->component.parent = parent;
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
	uic.onScreenResize = NULL;

	return uic;
}

void ui_destroyComponent(UIComponent* uic)
{
	free(uic->name);
	seqtor_destroy(uic->children);
}