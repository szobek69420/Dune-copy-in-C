#include "background.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../game_object.h"

struct Background {
	Transform transform;
	GameObjectFunctions functions;
};
typedef struct Background Background;


void background_destroy(void* background);
void background_update(void* background, float deltaTime);
void background_render(void* background);


extern void* MAIN_CUM;


void* background_create(const char* name)
{
	Background* bg = malloc(sizeof(Background));
	if(bg==NULL)
	{
		printf("Background: Background could not be created\n");
		return NULL;
	}

	bg->transform = gameObject_createTransform(name);
	
	memset(&bg->functions, 0, sizeof(GameObjectFunctions));
	bg->functions.destroy = background_destroy;
	bg->functions.render = background_render;
	bg->functions.onUpdate = background_update;


	return bg;
}

void background_destroy(void* background)
{
	Background* bg = background;
	gameObject_destroyTransform(&bg->transform);
	
	free(bg);
}

void background_update(void* background, float deltaTime)
{

}

void background_render(void* background)
{

}
