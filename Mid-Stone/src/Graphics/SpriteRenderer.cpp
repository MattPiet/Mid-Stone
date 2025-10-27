#include <Graphics/SpriteRenderer.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
void SpriteRenderer::loadImage(const char* path, int rows_ , int columns_, bool flip){
	//flip the image vertically so it is not upside down
    stbi_set_flip_vertically_on_load(flip);
	// tests if its a spritesheet cuz if rows and columns are NULL then its not a spritesheet
    if (rows_ && columns_) {
		rows = rows_;
		columns = columns_;
    }
	// load the image from file and set up width, height and number of color channels
    unsigned char* image = stbi_load(path, &imageWidth, &imageHeight, &comppp, 0);
    if (image == NULL)
    {
        printf("error loading image!\n");
    }
	// generate an OpenGL texture ID for this texture
    glGenTextures(1, &textureID);
    // bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);
	// basically if the texture is smaller or larger than the object its being applied to
    // also for testing for jpg and png
    GLenum format = (comppp == 4) ? GL_RGBA : GL_RGB;
    GLint internalFormat = (comppp == 4) ? GL_RGBA8 : GL_RGB8;
	// load the image data into the bound texture
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, image);

    
	// set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image); // free the image memory

}
std::pair<std::pair<float, float>, std::pair<float, float>> SpriteRenderer::runSpriteSheet(int current_sprite_index) const {
	// calculate the width and height of each cell in the spritesheet
    float cellWidth = 1.0f / static_cast<float>(columns);
    float cellHeight = 1.0f / static_cast<float>(rows);
	// calculate the current row and column based on the current sprite index
    int currentSpriteCol = current_sprite_index % columns;
    int currentSpriteRow = current_sprite_index / columns;
	// calculate the UV offset for the current sprite
    float uOffset = static_cast<float>(currentSpriteCol) * cellWidth;
    float vOffset = static_cast<float>(currentSpriteRow) * cellHeight;
	// return the UV scale and offset as a pair of pairs
        return std::make_pair(std::make_pair(cellWidth, cellHeight), std::make_pair(uOffset, vOffset));
}
void SpriteRenderer::renderSprite(Shader* shader, SpriteMesh* sprite_mesh, Matrix4 modelMatrix, int current_sprite_index) const{
	// Ok so this is all trippy but basically we bind the texture and set the uniform for the texture

    glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(shader->GetUniformID("ourTexture"), 0); // set the texture uniform to texture unit 0
	if (rows > 0 && columns > 0) { // if its a spritesheet
        std::pair sprite_info = runSpriteSheet(current_sprite_index); 
        glUniform2f(shader->GetUniformID("uvScale"), sprite_info.first.first, sprite_info.first.second);
        glUniform2f(shader->GetUniformID("uvOffset"), sprite_info.second.first, sprite_info.second.second);

        float frameWidth = (float)imageWidth / columns;
        float frameHeight = (float)imageHeight / rows;
        float aspect = frameWidth / frameHeight;
        float desiredHeight = 64.0f / 5.0f; // Aspect Ratio Pixels
        float desiredWidth = desiredHeight * aspect;
        modelMatrix *= MMath::scale(desiredWidth, desiredHeight, 1.0f);
    }
	else { // if its not a spritesheet
        glUniform2f(shader->GetUniformID("uvScale"), 1.0f, 1.0f);
		glUniform2f(shader->GetUniformID("uvOffset"), 0.0f, 0.0f);
        float aspect = (float)imageWidth / imageHeight;
        float desiredHeight = 64.0f / 10.0f;                   
        float desiredWidth = desiredHeight * aspect;  
        modelMatrix *= MMath::scale(desiredWidth, desiredHeight, 1.0f);
    }
	// now we need to scale the model matrix to the size of the image

    glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);
	// finally render the sprite mesh
    sprite_mesh->RenderMesh();
    glBindTexture(GL_TEXTURE_2D, 0); 
}
#endif