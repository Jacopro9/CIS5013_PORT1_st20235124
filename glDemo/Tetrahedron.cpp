#include "Tetrahedron.h"


using namespace std;
using namespace glm;


// Example data for tetrahedron model

// Packed vertex position buffer
static float positionArray[] = {

	0.0f, 2.0f, -0.25f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 0.0f, -1.0f, 1.0f,
	1.0f, 0.0f, -1.0f, 1.0f
};

// Packed colour buffer
static float colourArray[] = {

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f
};


// Triangle topology to render tetrahedron
static unsigned int indexArray[] = {

	// Top face
	2, 1, 0,
	2, 0, 3,
	3, 0, 1,
	3, 1, 2
};



Tetrahedron::Tetrahedron() {

	// setup vbo for position attribute
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionArray), positionArray, GL_STATIC_DRAW);

	// setup vbo for colour attribute
	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colourArray), colourArray, GL_STATIC_DRAW);

	// setup vbo for cube) index buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
}


Tetrahedron::~Tetrahedron() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


void Tetrahedron::render() {

	glShadeModel(GL_FLAT);

	// Setup VBOs ready for rendering
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// Declare which buffers / slots are needed
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Bind (and leave bound) the index array for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (const GLvoid*)0);

	// Once done unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Restore default shade model
	glShadeModel(GL_SMOOTH);
}
