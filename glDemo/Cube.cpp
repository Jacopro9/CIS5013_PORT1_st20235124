#include "Cube.h"


using namespace std;
using namespace glm;


// Example data for cube model

// Packed vertex buffer for cube
static float positionArray[] = {

	-1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,

	-1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f
};

// Packed colour buffer for principle axes model
static float colourArray[] = {

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};


// Line list topology to render principle axes
static unsigned int indexArray[] = {

	// Top face
	2, 1, 0,  
	3, 2, 0,

	// Bottom face
	5, 6, 4, 
	6, 7, 4,

	// Right face
	3, 7, 2, 
	7, 6, 2,

	// Front face
	0, 4, 3,
	4, 7, 3,

	// Left face
	0, 1, 5,
	4, 0, 5,
	
	// Back face
	2, 6, 1,
	6, 5, 1
};



Cube::Cube() {

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


Cube::~Cube() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


void Cube::render() {
	
	glShadeModel(GL_FLAT);

	// Setup VBOs ready for rendering
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	//glColorPointer(4, GL_FLOAT, 0, (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// Declare which arrays are needed for the semi-circle object
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Bind (and leave bound) the index array for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (const GLvoid*)0);

	// Declare which arrays are needed for the semi-circle object
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Restore default shade model
	glShadeModel(GL_SMOOTH);
}
