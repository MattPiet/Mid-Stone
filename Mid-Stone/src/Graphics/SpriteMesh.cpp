#include <Graphics/SpriteMesh.h>

void SpriteMesh::OnCreate(){
#define VERTEX_LENGTH 	(numVerts * (vertPropertiesCount * sizeof(float)))
#define TEXCOORD_LENGTH (numVerts * (vertPropertiesCount * sizeof(float)))
#define INDEX_LENGTH   (numIndices * sizeof(unsigned int))

	unsigned int verticiesLayoutLocation = 0;
	unsigned int texCoordsLayoutLocation = 1;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
	// vertex buffer
    glGenBuffers(1, &VertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH, vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(verticiesLayoutLocation);
    glVertexAttribPointer(verticiesLayoutLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// texture coord attribute and buffer
    glGenBuffers(1, &TexCoordsBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferID);
    glBufferData(GL_ARRAY_BUFFER, TEXCOORD_LENGTH, texCoords, GL_STATIC_DRAW);

	// Update texture coordinates if needed // honestly I dont think this is needed here but its good to know how to do it
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
    // bind the VAO, VAO means Vertex Array Object which is a collection of VBOs, VBO means Vertex Buffer Objects kinda trippy I know
	glBindVertexArray(VAO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 6 indices to draw 2 triangles
	glBindVertexArray(0);
}
void SpriteMesh::OnDestroy(){
	// clean up the buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VertexBufferID);
	glDeleteBuffers(1, &TexCoordsBufferID);
    glDeleteBuffers(1, &IndexBufferID);
}
