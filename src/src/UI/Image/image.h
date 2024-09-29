#ifndef IMAGE_H
#define IMAGE_H

void* image_create(const char* name);

void image_setScreenSize(int width, int height);

void image_setTexturePath(void* element, const char* texturePath,int channelCount);
void image_setTextureId(void* element, unsigned int textureId);
void image_setColour(void* element, float r, float g, float b, float a);

void image_getColour(void* element, float* r, float* g, float* b, float* a);

#endif