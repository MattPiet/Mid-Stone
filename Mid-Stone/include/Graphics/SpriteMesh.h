#ifndef SPRITEMESH_H
#define SPRITEMESH_H


#include <glew.h>

class SpriteMesh
{
    unsigned int VertexBufferID, VAO, IndexBufferID, TexCoordsBufferID;
    static constexpr int vertPropertiesCount = 2; 
    float vertices[8] = {
        // positions  
         0.5f,  0.5f,   // top right
         0.5f, -0.5f,   // bottom right
        -0.5f, -0.5f,   // bottom left
        -0.5f,  0.5f,    // top left 
    };
    static constexpr int numVerts = 4;
    static constexpr int numIndices = 6;
    unsigned int indices[6] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    float texCoords[8] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
    };

public:
    void OnCreate();
	void RenderMesh();
    void OnDestroy();
};
#endif // !SPRITEMESH_H