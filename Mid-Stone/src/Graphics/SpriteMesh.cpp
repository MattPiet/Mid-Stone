#include <Graphics/SpriteMesh.h>

void SpriteMesh::OnCreate(){
#define VERTEX_LENGTH 	(numVerts * (vertPropertiesCount * sizeof(float)))
#define TEXCOORD_LENGTH (numVerts * (vertPropertiesCount * sizeof(float)))
#define INDEX_LENGTH   (numIndices * sizeof(unsigned int))

	unsigned int verticiesLayoutLocation = 0;
	unsigned int texCoordsLayoutLocation = 1;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH, vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(verticiesLayoutLocation);
    glVertexAttribPointer(verticiesLayoutLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // texture coords
    glGenBuffers(1, &TexCoordsBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferID);
    glBufferData(GL_ARRAY_BUFFER, TEXCOORD_LENGTH, texCoords, GL_STATIC_DRAW);

	// Update texture coordinates if needed
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, TEXCOORD_LENGTH, texCoords);

    glEnableVertexAttribArray(texCoordsLayoutLocation);
    glVertexAttribPointer(texCoordsLayoutLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Index buffer
    glGenBuffers(1, &IndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_LENGTH, indices, GL_STATIC_DRAW);
}



void SpriteMesh::RenderMesh(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void SpriteMesh::OnDestroy(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VertexBufferID);
    glDeleteBuffers(1, &IndexBufferID);
}
