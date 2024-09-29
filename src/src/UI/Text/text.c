#define _CRT_SECURE_NO_WARNINGS

#include "text.h"
#include "../ui.h"
#include "../../Renderer/Fonts/fonts.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Text {
	UIComponent component;
	char* text;
	int fontSize;
	float r, g, b, a;
	TextOrigin hOrigin, vOrigin;
};
typedef struct Text Text;

static FontSet* FONT = NULL;
static int INSTANCE_COUNT = 0;

void text_render(void* text, int minX, int minY, int maxX, int maxY);
void text_destroy(void* text);

void* text_create(const char* name)
{
	Text* text = malloc(sizeof(Text));
	if (text == NULL)
		return NULL;

	text->component = ui_initComponent(name);
	text->component.render = text_render;
	text->component.destroy = text_destroy;

	text->text = malloc(sizeof(char) * 2);
	strcpy(text->text, "");

	text->fontSize = 40;

	text->hOrigin = ORIGIN_CENTER;
	text->vOrigin = ORIGIN_CENTER;

	text->r = 1;
	text->g = 1;
	text->b = 1;
	text->a = 1;

	//------------------
	if(INSTANCE_COUNT==0)
		FONT = fonts_import("Assets/Fonts/monocraft.png", "Assets/Fonts/monocraft.globus");
	INSTANCE_COUNT++;


	return text;
}


void text_destroy(void* text)
{
	Text* t = text;
	ui_destroyComponent(&t->component);
	free(t->text);
	free(t);
}

void text_render(void* text, int minX, int minY, int maxX, int maxY)
{
	Text* t = text;

	int currentX = minX;
	int currentY = minY;

	ui_calculateBounds(t, &currentX, &currentY, &maxX, &maxY);


	//currentX and currentY also has to be modified according to the text origin
	switch (t->hOrigin)
	{
	case ORIGIN_CENTER:
		currentX += t->component.width / 2;
		break;

	case ORIGIN_RIGHT:
		currentX += t->component.width;
		break;
	}

	switch (t->vOrigin)
	{
	case ORIGIN_CENTER:
		currentY += t->component.height / 2;
		break;

	case ORIGIN_BOTTOM:
		currentY += t->component.height;
		break;
	}

	fonts_setCurrentFont(FONT);
	fonts_setFontSize(t->fontSize);
	fonts_setColour(t->r, t->g, t->b, t->a);
	fonts_setOrigin(t->hOrigin, t->vOrigin);
	fonts_drawText(t->text, currentX, currentY);
}


void text_setText(void* element, const char* text)
{
	Text* t = element;
	free(t->text);
	t->text = malloc((strlen(t->text) + 1) * sizeof(char));
	strcpy(t->text, text);
}

void text_setFontSize(void* element, int fontSize)
{
	Text* t = element;
	t->fontSize = fontSize;
}

void text_setOrigin(void* element, TextOrigin hOrigin, TextOrigin vOrigin)
{
	Text* t = element;
	t->hOrigin = hOrigin;
	t->vOrigin = vOrigin;
}

void text_setColour(void* element, float r, float g, float b, float a)
{
	Text* t = element;
	t->r = r;
	t->g = g;
	t->b = b;
	t->a = a;
}

void text_getColour(void* element, float* r, float* g, float* b, float* a)
{
	Text* t = element;
	*r = t->r;
	*g = t->g;
	*b = t->b;
	*a = t->a;
}

void text_pack(void* element)//calculates the minimal size so that the text is in the element
{
	Text* t = element;
	fonts_setCurrentFont(FONT);
	fonts_setFontSize(t->fontSize);
	t->component.width = fonts_getTextWidth(t->text);
	t->component.height = fonts_getTextHeight(t->text, 0);
}