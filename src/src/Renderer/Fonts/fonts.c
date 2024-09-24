#define _CRT_SECURE_NO_WARNINGS

#include "fonts.h"

#include "../Texture/texture_handler.h"

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
	}

	fclose(meta);

	if (0)
	{
	DAYUM1:
		textureHandler_deleteImage(fs->texture);
	DAYUM2:
		free(fs);
	}

	printf("Font: %s loaded, %d characters\n", fs->name, seqtor_size(fs->characters));

	return fs;
}


void fonts_delete(FontSet* fs)
{
	free(fs->name);
	seqtor_destroy(fs->characters);
	textureHandler_deleteImage(fs->texture);
	free(fs);
}
