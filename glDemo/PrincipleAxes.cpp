#include "PrincipleAxes.h"

using namespace std;
using namespace glm;

// Example data for principle axes

// Packed vertex buffer for principle axes model
static float positionArray[] = {

	// x axis model
	0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.025f, 0.025f, 0.0, 1.0f,
	1.075f, 0.075f, 0.0, 1.0f,
	1.025f, 0.075f, 0.0, 1.0f,
	1.075f, 0.025f, 0.0, 1.0f,

	// y axis model
	0.0, 0.0, 0.0, 1.0f,
	0.0, 1.0, 0.0, 1.0f,
	-0.075f, 1.075f, 0.0, 1.0f,
	-0.05f, 1.05f, 0.0, 1.0f,
	-0.025f, 1.075f, 0.0, 1.0f,
	-0.075f, 1.025f, 0.0, 1.0f,

	// z axis model
	0.0, 0.0, 0.0, 1.0f,
	0.0, 0.0, 1.0, 1.0f,
	0.025f, 0.075f, 1.0, 1.0f,
	0.075f, 0.075f, 1.0, 1.0f,
	0.025f, 0.025f, 1.0, 1.0f,
	0.075f, 0.025f, 1.0, 1.0f
};

// Packed colour buffer for principle axes model
static float colourArray[] = {

	// x axis colour (red)
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	// y axis colour (green)
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	// z axis colour (blue)
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f
};


// Line list topology to render principle axes
static unsigned int indexArray[] = {

	0, 1, 2, 3, 4, 5,					// x axis
	6, 7, 8, 9, 10, 11,					// y axis
	12, 13, 14, 15, 15, 16, 16, 17		// z axis
};



CGPrincipleAxes::CGPrincipleAxes() {

	// setup vbo for position attribute
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionArray), positionArray, GL_STATIC_DRAW);

	// setup vbo for colour attribute
	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colourArray), colourArray, GL_STATIC_DRAW);

	// setup vbo for principle axis (pa) index buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
}


CGPrincipleAxes::~CGPrincipleAxes() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


void CGPrincipleAxes::render() {

	// Setup VBOs ready for rendering
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	//glColorPointer(4, GL_FLOAT, 0, (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);
	
	// Declare which arrays are needed for the semi-circle object
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Bind (and leave bound) the index array for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_LINES, 20, GL_UNSIGNED_INT, (const GLvoid*)0);

	// Declare which arrays are needed for the semi-circle object
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
