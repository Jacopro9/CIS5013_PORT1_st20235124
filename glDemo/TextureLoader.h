#pragma once

#include "core.h"

// Helper function for loading texture images from disk and setup a texture with defaut properties
GLuint loadTexture(std::string filename, FREE_IMAGE_FORMAT srcImageType);
