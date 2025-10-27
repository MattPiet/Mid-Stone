#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <glew.h>
#include <iostream>
#include <Graphics/Shader.h>
#include <Graphics/SpriteMesh.h>
#include <MMath.h>

class SpriteRenderer
{
	int imageWidth; // these are the width and height of the image in pixels
    int imageHeight;
	int comppp; // this is the number of color channels in the image
	unsigned int textureID; // the OpenGL texture ID used to bind the texture
	int rows, columns; // number of rows and columns in the spritesheet, if 0 then not a spritesheet
public:

	void loadImage(const char* path, int rows_ = NULL, int columns_ = NULL, bool flip = true); // load an image from file and create an OpenGL texture
	unsigned int getTextureID() const { return textureID; } // get the OpenGL texture ID

	// now I know this is a bit messy but it returns a pair of pairs its honestly a lot cleaner in use and way simpler once you get your head around it
	std::pair<std::pair<float, float>, std::pair<float, float>> runSpriteSheet(int current_sprite_index) const; // get the UV scale and offset for the current sprite index in the spritesheet

	// render the sprite with the given shader and model matrix, if current_sprite_index is 0 then render the whole image
	void renderSprite(Shader* shader, SpriteMesh* sprite_mesh, Matrix4 modelMatrix, int current_sprite_index = 0) const; 

	SpriteRenderer() : imageWidth{ 0 }, imageHeight{ 0 }, comppp{ 0 }, textureID{ 0 }, rows{ 0 }, columns{ 0 } {}

	int GetImageWidth() const { return imageWidth; }
	int GetImageHeight() const { return imageHeight; }
	int GetRows() const { return rows; }
	int GetColumns() const { return columns; }

};
#endif 