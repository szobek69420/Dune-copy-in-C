#define _CRT_SECURE_NO_WARNINGS

#include "button.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ui.h"
#include "../Text/text.h"
#include "../Image/image.h"

struct Colour {
	float r, g, b, a;
};
typedef struct Colour Colour;

struct Button {
	UIComponent component;

	void* text;
	void* image;

	Colour normalColour;
	Colour hoverColour;
	Colour pressedColour;

	//the corresponding functions in button::component are set by default and they call these if set
	void (*onPress)(void*);
	void (*onHold)(void*);
	void (*onRelease)(void*);
	void (*onHover)(void*);

	char state;//from left to right: ISHOVERED, ISHELD, ISSIZECHANGED ...
};
typedef struct Button Button;

#define IS_HOVERED(BUTTON_STATE) ((BUTTON_STATE)&0b10000000)
#define IS_HELD(BUTTON_STATE) ((BUTTON_STATE)&0b01000000)
#define SIZE_CHANGED(BUTTON_STATE) ((BUTTON_STATE)&0b00100000)

#define SET_HOVERED(BUTTON_STATE) (BUTTON_STATE)|=0b10000000;
#define SET_HELD(BUTTON_STATE) (BUTTON_STATE)|=0b01000000;
#define SET_SIZE_CHANGED(BUTTON_STATE) (BUTTON_STATE)|=0b00100000;

#define CLEAR_STATE(BUTTON_STATE) (BUTTON_STATE)=0b00000000;


void button_destroy(void* element);
void button_render(void* element, int minX, int minY, int maxX, int maxY);

void button_onPressDefault(void* element);
void button_onHoldDefault(void* element);
void button_onReleaseDefault(void* element);
void button_onHoverDefault(void* element);


void* button_create(const char* name)
{
	Button* button = malloc(sizeof(Button));
	if (button == NULL)
	{
		printf("Button: Button could not be created\n");
		return NULL;
	}

	//allocating children start
	char buffer[200];

	strcpy_s(buffer, 200, name);
	strcat_s(buffer, 200, "_text");
	button->text = ui_createElement(UI_TEXT, buffer);
	if (button->text == NULL)
	{
		printf("Button: Text could not be created\n");
		free(button);
		return NULL;
	}
	text_setText(button->text,"button");


	strcpy_s(buffer, 200, name);
	strcat_s(buffer, 200, "_image");
	button->image = ui_createElement(UI_IMAGE, buffer);
	if (button->image == NULL)
	{
		printf("Button: Image could not be created\n");
		
		Button* text = button->text;
		if (text->component.destroy != NULL)
			text->component.destroy(text);

		free(button);
		return NULL;
	}
	// allocating children end

	button->component = ui_initComponent(name);

	button->component.width = 100;
	button->component.height = 100;
	button->normalColour = (Colour){ 1.0f,1.0f,1.0f,1.0f };
	button->hoverColour = (Colour){ 0.8f,0.8f,0.8f,0.8f };
	button->pressedColour = (Colour){ 0.6f,0.6f, 0.6f, 0.6f };

	button->component.render = button_render;
	button->component.destroy = button_destroy;
	button->component.onPress = button_onPressDefault;
	button->component.onHold = button_onHoldDefault;
	button->component.onRelease = button_onReleaseDefault;
	button->component.onHover = button_onHoverDefault;

	button->onPress = NULL;
	button->onHold = NULL;
	button->onRelease = NULL;
	button->onHover = NULL;

	CLEAR_STATE(button->state);
	SET_SIZE_CHANGED(button->state);

	return button;
}

void button_destroy(void* element)
{
	Button* button = element;

	ui_destroyElement(button->text);
	ui_destroyElement(button->image);

	ui_destroyComponent(&button->component);
	free(button);
}

void button_render(void* element, int minX, int minY, int maxX, int maxY)
{
	Button* button = element;

	//check if the children are the same size as the button
	if (SIZE_CHANGED(button->state) != 0)
	{
		Button* text = button->text;
		text->component.width = button->component.width;
		text->component.height = button->component.height;
		Button* image = button->image;
		image->component.width = button->component.width;
		image->component.height = button->component.height;
	}


	//calculating position
	ui_calculateBounds(button, &minX, &minY, &maxX, &maxY);

	//handle user interaction
	Colour previousTextColour;
	Colour previousImageColour;
	int colourChanged = 0;


	if (IS_HELD(button->state))
	{
		text_getColour(button->text, &previousTextColour.r, &previousTextColour.g, &previousTextColour.b, &previousTextColour.a);
		image_getColour(button->image, &previousImageColour.r, &previousImageColour.g, &previousImageColour.b, &previousImageColour.a);
		colourChanged = 69;

		Colour temp;

		temp.r = button->pressedColour.r * previousTextColour.r;
		temp.g = button->pressedColour.g * previousTextColour.g;
		temp.b = button->pressedColour.b * previousTextColour.b;
		temp.a = button->pressedColour.a * previousTextColour.a;
		text_setColour(button->text, temp.r, temp.g, temp.b, temp.a);

		temp.r = button->pressedColour.r * previousImageColour.r;
		temp.g = button->pressedColour.g * previousImageColour.g;
		temp.b = button->pressedColour.b * previousImageColour.b;
		temp.a = button->pressedColour.a * previousImageColour.a;
		image_setColour(button->image, temp.r, temp.g, temp.b, temp.a);
	}
	else if (IS_HOVERED(button->state))
	{
		text_getColour(button->text, &previousTextColour.r, &previousTextColour.g, &previousTextColour.b, &previousTextColour.a);
		image_getColour(button->image, &previousImageColour.r, &previousImageColour.g, &previousImageColour.b, &previousImageColour.a);
		colourChanged = 69;

		Colour temp;

		temp.r = button->hoverColour.r * previousTextColour.r;
		temp.g = button->hoverColour.g * previousTextColour.g;
		temp.b = button->hoverColour.b * previousTextColour.b;
		temp.a = button->hoverColour.a * previousTextColour.a;
		text_setColour(button->text, temp.r, temp.g, temp.b, temp.a);

		temp.r = button->hoverColour.r * previousImageColour.r;
		temp.g = button->hoverColour.g * previousImageColour.g;
		temp.b = button->hoverColour.b * previousImageColour.b;
		temp.a = button->hoverColour.a * previousImageColour.a;
		image_setColour(button->image, temp.r, temp.g, temp.b, temp.a);
	}

	//render elements
	Button* globus = button->image;
	if (globus->component.render != NULL && globus->component.isVisible != 0)
		globus->component.render(globus, minX, minY, maxX, maxY);
	
	globus = button->text;
	if (globus->component.render != NULL && globus->component.isVisible != 0)
		globus->component.render(globus, minX, minY, maxX, maxY);


	//minden utan
	if (colourChanged != 0)
	{
		text_setColour(button->text, previousTextColour.r, previousTextColour.g, previousTextColour.b, previousTextColour.a);
		image_setColour(button->image, previousImageColour.r, previousImageColour.g, previousImageColour.b, previousImageColour.a);
	}


	CLEAR_STATE(button->state)
}

void* button_getBackgroundImage(void* element)
{
	return ((Button*)element)->image;
}
void* button_getText(void* element)
{
	return ((Button*)element)->text;
}

void button_setNormalColour(void* element, float r, float g, float b, float a)
{
	Button* button=element;
	button->normalColour.r = r;
	button->normalColour.g = g;
	button->normalColour.b = b;
	button->normalColour.a = a;
}
void button_setHoverColour(void* element, float r, float g, float b, float a)
{
	Button* button = element;
	button->hoverColour.r = r;
	button->hoverColour.g = g;
	button->hoverColour.b = b;
	button->hoverColour.a = a;
}
void button_setPressedColour(void* element, float r, float g, float b, float a)
{
	Button* button = element;
	button->pressedColour.r = r;
	button->pressedColour.g = g;
	button->pressedColour.b = b;
	button->pressedColour.a = a;
}


void button_setOnPress(void* element, void(*onPress)(void*))
{
	((Button*)element)->onPress = onPress;
}

void button_setOnHold(void* element, void(*onHold)(void*))
{
	((Button*)element)->onHold = onHold;
}

void button_setOnRelease(void* element, void(*onRelease)(void*))
{
	((Button*)element)->onRelease = onRelease;
}

void button_setOnHover(void* element, void(*onHover)(void*))
{
	((Button*)element)->onHover = onHover;
}


void button_onPressDefault(void* element)
{
	Button* button = element;

	SET_HELD(button->state);

	if (button->onPress != NULL)
		button->onPress(button);
}

void button_onHoldDefault(void* element)
{
	Button* button = element;

	SET_HELD(button->state);

	if (button->onHold != NULL)
		button->onHold(button);
}


void button_onReleaseDefault(void* element)
{
	Button* button = element;

	SET_HELD(button->state);

	if (button->onRelease != NULL)
		button->onRelease(button);
}

void button_onHoverDefault(void* element)
{
	Button* button = element;

	SET_HOVERED(button->state);

	if (button->onHover != NULL)
		button->onHover(button);
}