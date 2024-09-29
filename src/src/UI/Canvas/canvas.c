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
		ui_calculateBounds(canvas, &minX, &minY, &maxX, &maxY);


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