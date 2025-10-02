#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <glew.h>
#include <iostream>

class SpriteRenderer
{
    int imageWidth;
    int imageHeight;
    int comppp;
    unsigned int textureID;
public:

    void loadImage(const char* path, bool flip = true);
    std::pair<std::pair<float, float>, std::pair<float, float>> runSpriteSheet(int rows, int columns, int current_sprite_index);
	unsigned int getTextureID() const { return textureID;}

	SpriteRenderer() : imageWidth{ 0 }, imageHeight{ 0 }, comppp{ 0 }, textureID{ 0 } {}
};
#endif 