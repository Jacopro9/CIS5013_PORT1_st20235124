#pragma once

#include "core.h"

class AIMesh {

	GLuint				numFaces = 0;

	GLuint				vao = 0;

	GLuint				meshVertexPosBuffer = 0;
	GLuint				meshTexCoordBuffer = 0;
	
	GLuint				meshNormalBuffer = 0; // surface basis z
	GLuint				meshTangentBuffer = 0; // surface basis x (u aligned)
	GLuint				meshBiTangentBuffer = 0; // surface basis y (v aligned)

	GLuint				meshFaceIndexBuffer = 0;

	GLuint				textureID = 0;
	GLuint				normalMapID = 0;

	// Private functions
	void setupGLStuff(aiMesh* mesh);

public:

	AIMesh(std::string filename, GLuint meshIndex = 0);
	AIMesh(const struct aiScene* scene, GLuint meshIndex = 0);

	void addTexture(GLuint textureID);
	void addTexture(std::string filename, FREE_IMAGE_FORMAT format);

	void addNormalMap(GLuint normalMapID);
	void addNormalMap(std::string filename, FREE_IMAGE_FORMAT format);

	void setupTextures();
	void render();
};