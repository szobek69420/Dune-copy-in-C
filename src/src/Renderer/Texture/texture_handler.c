#include <glad/glad.h>
#include <stb_image.h>
#include <stdio.h>
#include "texture_handler.h"
#include <seqtor.h>

static seqtor_of(texture_t) loadedTextures;

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

    seqtor_push_back(loadedTextures, texture);

    return texture;
}

void textureHandler_deleteImage(texture_t texture)
{
    seqtor_remove(loadedTextures, texture);
    glDeleteTextures(1, &texture);
}