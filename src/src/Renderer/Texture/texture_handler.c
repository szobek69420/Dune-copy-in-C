#define _CRT_SECURE_NO_WARNINGS

#include "texture_handler.h"

#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <seqtor.h>


struct TextureInfo {
    texture_t id;

    char* path;
    GLint internalFormat;
    GLenum format;
    int filterType;
    int flipVertically;

    int referenceCount;
};
typedef struct TextureInfo TextureInfo;

static seqtor_of(struct TextureInfo) loadedTextures;

void textureHandler_init()
{
    seqtor_init(loadedTextures, 1);
}

void textureHandler_deinit()
{
    for (int i = 0; i < seqtor_size(loadedTextures); i++)
        glDeleteTextures(1,&(seqtor_at(loadedTextures,i)));
    seqtor_destroy(loadedTextures, 1);
}

unsigned int textureHandler_loadImage(const char* pathToTexture, GLint internalFormat, GLenum format, int filterType, int flipVertically)
{
    for (int i = 0; i < seqtor_size(loadedTextures); i++)
    {
#define _t (seqtor_at(loadedTextures,i))
        if (strcmp(pathToTexture, _t.path) == 0 &&
            internalFormat == _t.internalFormat &&
            format == _t.format &&
            filterType == _t.filterType &&
            flipVertically == _t.flipVertically)
        {
            _t.referenceCount++;
            return _t.id;
        }

#undef _t
    }

    stbi_set_flip_vertically_on_load(flipVertically);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(pathToTexture, &width, &height, &nrChannels, 0);
    if (data!=NULL)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
    }
    else
    {
        printf("Failed to load the texture %s\n", pathToTexture);
        return 0;
    }
    stbi_image_free(data);

    TextureInfo ti;
    ti.id = texture;
    ti.internalFormat = internalFormat;
    ti.format = format;
    ti.filterType = filterType;
    ti.flipVertically = flipVertically;
    ti.path = (char*)malloc((strlen(pathToTexture) + 1) * sizeof(char));
    strcpy(ti.path, pathToTexture);
    ti.referenceCount = 1;

    seqtor_push_back(loadedTextures, ti);

    return texture;
}

void textureHandler_deleteImage(texture_t texture)
{
    for (int i = 0; i < seqtor_size(loadedTextures); i++)
    {
#define _t ((TextureInfo*)&(seqtor_at(loadedTextures,i)))
        if (_t->id != texture)
            continue;

        _t->referenceCount--;
        if (_t->referenceCount == 0)
        {
            free(_t->path);
            glDeleteTextures(1, &(_t->id));
            seqtor_remove_at(loadedTextures, i);
        }
        break;
#undef _t
    }
}