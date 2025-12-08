#ifndef TEXTURE_H
#define TEXTURE_H
#include <glew.h>
class Texture {
	GLuint textureID;
public:
	Texture();
	~Texture();
	bool LoadImage(const char* filename);

	//inline makes it so the code just go and call the return of the function directly instead of 
	//calling the function, making it more efficient.
	inline GLuint getTextureID() const { return textureID; }


};
#endif

