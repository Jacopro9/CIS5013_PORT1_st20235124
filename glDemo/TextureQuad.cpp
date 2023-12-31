
#include "TextureQuad.h"


#pragma region Geometry data to setup VBOs for rendering - use triangles not quads here

static float positions[] = {

	-1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, -1.0f,
	-1.0f, -1.0f
};

static float texCoords[] = {

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f
};

// Indices for 2 triangles
static GLuint indices[] = { 0, 2, 1, 0, 3, 2 };

#pragma endregion


// VBOs for quad geometry used for each planetary body
static GLuint posVBO;
static GLuint texCoordVBO;
static GLuint indicesVBO;


void setupTextureQuadVBO() {

	// Setup VBOs for quad geometry
	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glGenBuffers(1, &texCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind buffers once done
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Setup state for known batch of objects using quad
void textureQuadPreRender() {

	// Setup VBOs ready for rendering
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)0);

	// Bind (and leave bound) the index array for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);

	// Declare which arrays are needed for the semi-circle object
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void textureQuadRender() {

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
}

void textureQuadPostRender() {

	// Disable vertex arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Unbind buffers once done
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}