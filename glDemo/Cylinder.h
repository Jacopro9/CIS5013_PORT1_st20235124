#pragma once

#include "AIMesh.h"

class Cylinder : public AIMesh {

	// Texture objects for force field effect
	// note: don't use inherited texture for these!
	GLuint wave1Texture = 0;
	GLuint wave2Texture = 0;

	// Specific shader to render cylinder (and it's effect)
	GLuint shader = 0;

	GLint shader_mvpMatrix = -1;
	GLint shader_wave1Texture = -1;
	GLint shader_wave2Texture = -1;
	GLint shader_wave1Phase = -1;
	GLint shader_wave2Phase = -1;

	float wavePhase = 0.0f;

public:

	Cylinder(std::string filename, GLuint meshIndex = 0);

	void setupTextures();
	void render(glm::mat4 transform);

	void update(float tDelta);
};