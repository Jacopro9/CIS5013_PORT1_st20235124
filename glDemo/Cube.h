#pragma once

#include "core.h"

class Cube {

private:

	GLuint					vertexBuffer;
	GLuint					colourBuffer;
	GLuint					indexBuffer;

public:

	Cube();
	~Cube();

	void render();
};