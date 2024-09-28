#include "canvas.h"

#include <stdlib.h>
#include <seqtor.h>
#include <stdio.h>

#include "../ui.h"
#include "../../Renderer/Window/window.h"

struct Canvas {
	UIComponent component;
	int autoScale;
};
typedef struct Canvas Canvas;


void canvas_destroy(void* element);
void canvas_render(void* element, int minX, int minY, int maxX, int maxY);
void canvas_onScreenResize(void* element, int width, int height);



void* canvas_create(const char* name, int autoScale)
{
	Canvas* canvas = malloc(sizeof(Canvas));
	if (canvas == NULL)
	{
		printf("Canvas: Canvas could not be created\n");
		return NULL;
	}

	canvas->component = ui_initComponent(name);

	canvas->autoScale = autoScale;
	if (autoScale != 0)
	{
		canvas->component.width = window_width();
		canvas->component.height = window_height();
	}

	canvas->component.destroy = canvas_destroy;
	canvas->component.render = canvas_render;
	canvas->component.onScreenResize = canvas_onScreenResize;

	return canvas;
}

void canvas_destroy(void* element)
{
	Canvas* canvas = element;
	ui_destroyComponent(&canvas->component);
	free(canvas);
}

void canvas_render(void* element, int minX, int minY, int maxX, int maxY)
{
	Canvas* canvas = element;

	if (canvas->autoScale)
	{
		minX = 0;
		minY = 0;
		maxX = canvas->component.width;
		maxY = canvas->component.height;
	}
	else
	{
		switch (canvas->component.hAlign)
		{
		case ALIGN_LEFT:
			minX = minX + canvas->component.xPos;
			break;

		case ALIGN_CENTER:
			minX = (minX + maxX) / 2 + canvas->component.xPos - canvas->component.width / 2;
			break;

		case ALIGN_RIGHT:
			minX = maxX - canvas->component.xPos - canvas->component.width;
			break;
		}

		switch (canvas->component.vAlign)
		{
		case ALIGN_TOP:
			minY = minY + canvas->component.yPos;
			break;

		case ALIGN_CENTER:
			minY = (minY + maxY) / 2 + canvas->component.yPos - canvas->component.height / 2;
			break;

		case ALIGN_BOTTOM:
			minY = maxY - canvas->component.yPos - canvas->component.height;
			break;
		}

		maxX = minX + canvas->component.width;
		maxY = minY + canvas->component.height;
	}


	for (int i = 0; i < seqtor_size(canvas->component.children); i++)
	{
		UIComponent* uic = seqtor_at(canvas->component.children, i);
		if(uic->isVisible&&uic->render!=NULL)
			uic->render(uic, minX, minY, maxX, maxY);
	}
}

void canvas_onScreenResize(void* element, int width, int height)
{
	Canvas* canvas = element;
	if (canvas->autoScale == 0)
		return;

	canvas->component.width = width;
	canvas->component.height = height;
	printf("resized to: %d %d\n", width, height);
}


void canvas_setAutoScale(void* element, int autoScale)
{
	Canvas* canvas = element;

	if (autoScale)
	{
		canvas->component.width = window_width();
		canvas->component.height = window_height();
		canvas->autoScale = autoScale;
		return;
	}

	canvas->autoScale = 0;
}