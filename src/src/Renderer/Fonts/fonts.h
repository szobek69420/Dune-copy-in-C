#ifndef FONTS_H
#define FONTS_H

struct FontSet;
typedef struct FontSet FontSet;

enum TextOrigin {
	ORIGIN_TOP, ORIGIN_CENTER, ORIGIN_BOTTOM,
	ORIGIN_LEFT, ORIGIN_RIGHT
};
typedef enum TextOrigin TextOrigin;

void fonts_init();
void fonts_deinit();

FontSet* fonts_import(const char* pathToFontImage, const char* pathToFontMeta);
void fonts_delete(FontSet* fs);

void fonts_drawText(const char* text, int x, int y);

void fonts_setScreenSize(int x, int y);
void fonts_setCurrentFont(FontSet* fs);

#endif