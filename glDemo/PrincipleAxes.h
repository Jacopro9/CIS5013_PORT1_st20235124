#pragma once

#include "core.h"



class CGPrincipleAxes  {

private:

	GLuint					vertexBuffer;
	GLuint					colourBuffer;
	GLuint					indexBuffer;

public:

	CGPrincipleAxes();
	~CGPrincipleAxes();

	void render();
};
