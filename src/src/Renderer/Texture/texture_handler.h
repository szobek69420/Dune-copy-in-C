#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

typedef unsigned int texture_t;

texture_t textureHandler_loadImage(const char* pathToTexture, GLint internalFormat, GLenum format, int filterType, int flipVertically);
void textureHandler_deleteImage(texture_t texture);

void textureHandler_init();
void textureHandler_deinit();

#endif