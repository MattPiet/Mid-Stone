#include <Graphics/SpriteRenderer.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
void SpriteRenderer::loadImage(const char* path, bool flip){
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* image = stbi_load(path, &imageWidth, &imageHeight, &comppp, 0);
    if (image == NULL)
    {
        printf("error loading image!\n");
    }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  int mode = (comppp == 4) ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, mode, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_image_free(image);

}
std::pair<std::pair<float, float>, std::pair<float, float>> SpriteRenderer::runSpriteSheet(int rows, int columns, int current_sprite_index){
    float  cellWidth = 1.0f / static_cast<float>(columns);
    float   cellHeight = 1.0f / static_cast<float>(rows);

    int  currentSpriteCol = current_sprite_index % columns;
    int  currentSpriteRow = current_sprite_index / columns;

    float  uOffset = static_cast<float>(currentSpriteCol) * cellWidth;
    float  vOffset = static_cast<float>(currentSpriteRow) * cellHeight;

	return std::make_pair(std::make_pair(cellWidth, cellHeight), std::make_pair(uOffset, vOffset));    

}
#endif