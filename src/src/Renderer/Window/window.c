#include "window.h"

static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 600;

int window_width()
{
	return WINDOW_WIDTH;
}
int window_height()
{
	return WINDOW_HEIGHT;
}

void window_set(int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}