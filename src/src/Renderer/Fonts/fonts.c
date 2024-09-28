#define _CRT_SECURE_NO_WARNINGS

#include "fonts.h"
#include "../../Glm2/mat4.h"
#include "../../Glm2/vec4.h"

#include "../Texture/texture_handler.h"
#include "../Shader/shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <seqtor.h>
#include <glad/glad.h>

struct Character {
	char id;
	float startX, startY;//top left uv coords
	float endX, endY;//bottom right uv coords

	int width, height;//in pixels
	int offsetX, offsetY;//offset from the top left of the character
};
typedef struct Character Character;

struct FontSet {
	unsigned int texture;
	char* name;
	int size;
	
	seqtor_of(Character) characters;
};
typedef struct FontSet FontSet;

#define ADJUST_SIZE(VAL) ((VAL)*CURRENT_FONT_SIZE/(CURRENT_FONT->size))

const int TEXT_ADVANCE = 1;

int compare(const void* char1, const void* char2);
int searchForCharacter(char id, Character* arr, int size);

//helper variables
static const float vertices[] = {
	0,0,0,1,1,1,1,0
};
static unsigned int VAO=0, VBO=0;
static unsigned int SHADER=0;

//state variables
static TextOrigin ORIGIN_V = ORIGIN_TOP, ORIGIN_H = ORIGIN_LEFT;
static FontSet* CURRENT_FONT = NULL;
static int CURRENT_FONT_SIZE = 40;
static int SCREEN_WIDTH = -1, SCREEN_HEIGHT = -1;
static Mat4 SCREEN_MATRIX;

void fonts_init()
{
	fonts_setScreenSize(1920, 1080);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), NULL);

	glBindVertexArray(0);

	SHADER=shader_import("Assets/Shaders/Fonts/fonts.vag", "Assets/Shaders/Fonts/fonts.fag", NULL);
	glUseProgram(SHADER);
	glUniform1i(glGetUniformLocation(SHADER, "tex"), 0);
	glUniform4f(glGetUniformLocation(SHADER, "colour"), 1, 1, 1, 1);
	glUniformMatrix4fv(glGetUniformLocation(SHADER, "projection"), 1, GL_FALSE, SCREEN_MATRIX.data);
	glUseProgram(0);
}

void fonts_deinit()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader_delete(SHADER);
}

FontSet* fonts_import(const char* fontImage, const char* fontMeta)
{
	FontSet* fs = malloc(sizeof(FontSet));

	fs->texture = textureHandler_loadImage(fontImage, GL_RGBA, GL_RGBA, GL_LINEAR, 69);
	if (fs->texture == 0)
		goto DAYUM2;
	
	FILE* meta = fopen(fontMeta, "r");
	if (meta == NULL)
		goto DAYUM1;

	char buffer[100];
	fscanf(meta, "%s", buffer);
	fs->name = malloc(sizeof(char) * (1 + strlen(buffer)));
	strcpy(fs->name, buffer);
	
	fscanf(meta, "%d", &fs->size);

	int width, height;
	fscanf(meta, "%d %d", &width, &height);
	fgetc(meta);
	
	seqtor_init(fs->characters, 1);
	while (69)
	{
		Character logus;
		int cx, cy;

		char* result=fgets(buffer, 100, meta);
		if (result == NULL || strlen(buffer) == 0)
			break;

		logus.id = buffer[0];

		sscanf(buffer+2, "%d,%d,%d,%d,%d,%d", &cx, &cy, &logus.width, &logus.height, &logus.offsetX, &logus.offsetY);

		logus.startX = (float)cx / width;
		logus.startY = 1 - ((float)cy / height);
		logus.endX = logus.startX + (float)logus.width / width;
		logus.endY = logus.startY - (float)logus.height / height;

		seqtor_push_back(fs->characters, logus);
		//printf("%c: %.2f,%.2f\n", logus.id, logus.startX, logus.startY);
	}

	fclose(meta);

	if (0)
	{
	DAYUM1:
		textureHandler_deleteImage(fs->texture);
	DAYUM2:
		free(fs);
	}

	qsort(fs->characters.data, fs->characters.size, sizeof(Character), compare);//hogy gyorsabb legyen benne a kereses
	
	printf("Font: %s loaded, %d characters\n", fs->name, seqtor_size(fs->characters));

	return fs;
}


void fonts_delete(FontSet* fs)
{
	if (fs == CURRENT_FONT)
		CURRENT_FONT = NULL;

	free(fs->name);
	seqtor_destroy(fs->characters);
	textureHandler_deleteImage(fs->texture);
	free(fs);
}


void fonts_drawText(const char* text, int x, int y)
{
	if (CURRENT_FONT == NULL)
	{
		printf("No font is selected\n");
		return;
	}

	int currentX = x;
	int currentY = SCREEN_HEIGHT-y;//the ui library's (0;0) is the top left corner, the font's is the bottom left

	switch (ORIGIN_H)
	{
	case ORIGIN_CENTER:
		do
		{
			int width = fonts_getTextWidth(text);
			currentX -= width / 2;
		} while (0);
		break;

	case ORIGIN_RIGHT:
		do
		{
			int width = fonts_getTextWidth(text);
			currentX -= width;
		} while (0);
		break;
	}

	switch (ORIGIN_V)
	{
	case ORIGIN_CENTER:
		do
		{
			int height = fonts_getTextHeight(text,0);
			currentY -= height / 2;
		} while (0);
		break;

	case ORIGIN_TOP:
		do
		{
			int height = fonts_getTextHeight(text, 69);
			currentY -= height;
		} while (0);
		break;
	}


	glUseProgram(SHADER);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CURRENT_FONT->texture);

	glBindVertexArray(VAO);

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);

	int advance = ADJUST_SIZE(TEXT_ADVANCE);
	if (advance < 1)
		advance = 1;

	while (*text != 0)
	{
		int index = searchForCharacter(*text, CURRENT_FONT->characters.data, CURRENT_FONT->characters.size);
		if (index == -1)
			continue;

		Character* current = CURRENT_FONT->characters.data + index;

		Vec4 textureInfo = (Vec4){ current->startX,current->endY,current->endX - current->startX,current->startY - current->endY };
		Vec4 screenInfo = { currentX + ADJUST_SIZE(current->offsetX),currentY + ADJUST_SIZE(current->offsetY),ADJUST_SIZE(current->width),ADJUST_SIZE(current->height)};

		glUniform4f(glGetUniformLocation(SHADER, "textureInfo"), textureInfo.x, textureInfo.y, textureInfo.z, textureInfo.w);
		glUniform4f(glGetUniformLocation(SHADER, "screenInfo"), screenInfo.x, screenInfo.y, screenInfo.z, screenInfo.w);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		currentX += ADJUST_SIZE(current->width) + advance;
		text++;
	}

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	glBindVertexArray(0);
	glUseProgram(0);
}


void fonts_setScreenSize(int x, int y)
{
	SCREEN_WIDTH = x;
	SCREEN_HEIGHT = y;

	SCREEN_MATRIX = mat4_ortho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);

	glUseProgram(SHADER);
	glUniformMatrix4fv(glGetUniformLocation(SHADER, "projection"), 1, GL_FALSE, SCREEN_MATRIX.data);
	glUseProgram(0);
}

void fonts_setCurrentFont(FontSet* fs)
{
	CURRENT_FONT = fs;
}

void fonts_setFontSize(int size)
{
	CURRENT_FONT_SIZE = size;
}
void fonts_setOrigin(TextOrigin horizontal, TextOrigin vertical)
{
	if (horizontal == ORIGIN_LEFT || horizontal == ORIGIN_CENTER || horizontal == ORIGIN_RIGHT)
		ORIGIN_H = horizontal;
	else
		printf("Fonts: Invalid horizontal origin\n");

	if (vertical==ORIGIN_TOP || vertical == ORIGIN_CENTER || vertical == ORIGIN_BOTTOM)
		ORIGIN_V = vertical;
	else
		printf("Fonts: Invalid horizontal origin\n");
}

void fonts_setColour(float r, float g, float b, float a)
{
	glUseProgram(SHADER);
	glUniform4f(glGetUniformLocation(SHADER, "colour"), r, g, b, a);
	glUseProgram(0);
}


int fonts_getTextWidth(const char* text)
{
	if (CURRENT_FONT == NULL)
	{
		printf("Fonts: No font is selected\n");
		return 0;
	}

	int length = 0;
	int advance = ADJUST_SIZE(TEXT_ADVANCE);
	if (advance < 1)
		advance = 1;

	while (*text != 0)
	{
		int index = searchForCharacter(*text, CURRENT_FONT->characters.data, CURRENT_FONT->characters.size);
		if (index == -1)
			continue;

		int charWidth = seqtor_at(CURRENT_FONT->characters, index).width;
		length += ADJUST_SIZE(charWidth) + advance;
		
		text++;
	}

	return length;
}

int fonts_getTextHeight(const char* text, int ignoreNegative)
{
	if (CURRENT_FONT == NULL)
	{
		printf("Fonts: No font is selected\n");
		return 0;
	}

	if (strlen(text) == 0)
		return 0;

	int minHeight = 1000000;
	int maxHeight = -1000000;
	while (*text != 0)
	{
		int index = searchForCharacter(*text, CURRENT_FONT->characters.data, CURRENT_FONT->characters.size);
		if (index == -1)
			continue;

		if (maxHeight < seqtor_at(CURRENT_FONT->characters, index).offsetY)
			maxHeight = seqtor_at(CURRENT_FONT->characters, index).offsetY;
		if (minHeight > seqtor_at(CURRENT_FONT->characters, index).offsetY - seqtor_at(CURRENT_FONT->characters, index).height)
			minHeight = seqtor_at(CURRENT_FONT->characters, index).offsetY - seqtor_at(CURRENT_FONT->characters, index).height;

		text++;
	}

	int height = ignoreNegative ? maxHeight : maxHeight - minHeight;
	height = ADJUST_SIZE(height);

	return height;
}





int compare(const void* char1, const void* char2)
{
	char id1 = ((Character*)char1)->id;
	char id2 = ((Character*)char2)->id;

	if (id1 > id2)
		return 69;
	if (id1 == id2)
		return 0;
	return -69;
}

int searchForCharacter(char id, Character* arr, int size)
{
	if (size == 0)
		return -1;

	int index = size / 2;

	int result;
	if (arr[index].id == id)
		return index;
	if (arr[index].id < id)
	{
		result=searchForCharacter(id, arr + index + 1, size - index - 1);
		if (result == -1)
			return -1;
		return result + index + 1;
	}
	return searchForCharacter(id, arr, index);
}