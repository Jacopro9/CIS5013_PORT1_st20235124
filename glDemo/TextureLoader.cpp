
#include "TextureLoader.h"

using namespace std;


// Utility function to load an image using FreeImage, convert to 32 bits-per-pixel (bpp) and setup and return a new texture object based on this.
GLuint loadTexture(string filename, FREE_IMAGE_FORMAT srcImageType) {

	// Load and validate bitmap
	FIBITMAP* loadedBitmap = FreeImage_Load(srcImageType, filename.c_str(), BMP_DEFAULT);

	if (!loadedBitmap) {

		cout << "FreeImage: Could not load image " << filename << endl;
		return 0;
	}

	// Comvert to RGBA format
	FIBITMAP* bitmap32bpp = FreeImage_ConvertTo32Bits(loadedBitmap);
	FreeImage_Unload(loadedBitmap);

	if (!bitmap32bpp) {

		cout << "FreeImage: Conversion to 32 bits unsuccessful for image " << filename << endl;
		return 0;
	}

	// Image loaded and converted - setup new texture object
	GLuint newTexture = 0;

	// If image loaded, setup new texture object in OpenGL
	glGenTextures(1, &newTexture); // can create more than 1!

	if (newTexture) {

		glBindTexture(GL_TEXTURE_2D, newTexture);

		// Setup texture image properties
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			FreeImage_GetWidth(bitmap32bpp),
			FreeImage_GetHeight(bitmap32bpp),
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE,
			FreeImage_GetBits(bitmap32bpp));

		// Setup texture filter and wrap properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}

	// Once the texture has been setup, the image data is copied into OpenGL.  We no longer need the originally loaded image
	FreeImage_Unload(bitmap32bpp);

	return newTexture;
}