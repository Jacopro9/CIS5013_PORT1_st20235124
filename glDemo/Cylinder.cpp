
#include "Cylinder.h"
#include "TextureLoader.h"
#include "shader_setup.h"

using namespace std;
using namespace glm;


Cylinder::Cylinder(std::string filename, GLuint meshIndex) : AIMesh(filename, meshIndex) {

	// Load textures
	wave1Texture = loadTexture("Assets\\cylinder\\waves1.png", FIF_PNG);
	wave2Texture = loadTexture("Assets\\cylinder\\waves2.png", FIF_PNG);

	// Load shader
	shader = setupShaders(string("Assets\\cylinder\\cylinder.vert"), string("Assets\\cylinder\\cylinder.frag"));

	// Get uniform locations
	shader_mvpMatrix = glGetUniformLocation(shader, "mvpMatrix");
	shader_wave1Texture = glGetUniformLocation(shader, "wave1Texture");
	shader_wave2Texture = glGetUniformLocation(shader, "wave2Texture");
	shader_wave1Phase = glGetUniformLocation(shader, "wave1Phase");
	shader_wave2Phase = glGetUniformLocation(shader, "wave2Phase");

	// Set uniform values that will not change (ie. texture sampler values)
	glUseProgram(shader);
	glUniform1i(shader_wave1Texture, 1);
	glUniform1i(shader_wave2Texture, 2);
	glUseProgram(0); // restore default
}

// Override pre and post render to use wave textures unique to cylinder

void Cylinder::setupTextures() {

	// Now bind wave textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wave1Texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, wave2Texture);

	// restore default
	glActiveTexture(GL_TEXTURE0);

}

void Cylinder::render(mat4 transform) {

	// Handle cylinder effects internally so setup shader here
	glUseProgram(shader);
	glUniformMatrix4fv(shader_mvpMatrix, 1, GL_FALSE, (GLfloat*)&transform);
	glUniform1f(shader_wave1Phase, cosf(glm::radians<float>(wavePhase)));
	glUniform1f(shader_wave2Phase, sinf(glm::radians<float>(wavePhase)));

	AIMesh::render();
}


void Cylinder::update(float tDelta) {

	wavePhase += 15.0f * tDelta;  // 30 degree per second

	if (wavePhase >= 360.0f) {
		 
		// wrap round to keep in [0, 360) range
		// assume we never jump >360 degrees in one frame :)
		wavePhase -= 360.0f;
	}
}

