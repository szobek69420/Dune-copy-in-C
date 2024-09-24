#ifndef FONTS_H
#define FONTS_H

struct FontSet;
typedef struct FontSet FontSet;


FontSet* fonts_import(const char* pathToFontImage, const char* pathToFontMeta);
void fonts_delete(FontSet* fs);

#endif