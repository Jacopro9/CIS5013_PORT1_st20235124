#pragma once

//
// Load and compile OpenGL Shading Language (GLSL) shaders
//

#include "core.h"


// Declare GLSL setup return / error codes
enum class ShaderError : uint8_t {

	GLSL_OK = 0,

	GLSL_SHADER_SOURCE_NOT_FOUND,
	GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND,
	GLSL_TESS_CONTROL_SHADER_SOURCE_NOT_FOUND,
	GLSL_TESS_EVALUATION_SHADER_SOURCE_NOT_FOUND,
	GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND,
	GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND,
	GLSL_COMPUTE_SHADER_SOURCE_NOT_FOUND,

	GLSL_SHADER_OBJECT_CREATION_ERROR,
	GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR,
	GLSL_TESS_CONTROL_SHADER_OBJECT_CREATION_ERROR,
	GLSL_TESS_EVALUATION_SHADER_OBJECT_CREATION_ERROR,
	GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR,
	GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR,
	GLSL_COMPUTE_SHADER_OBJECT_CREATION_ERROR,

	GLSL_SHADER_COMPILE_ERROR,
	GLSL_VERTEX_SHADER_COMPILE_ERROR,
	GLSL_TESS_CONTROL_SHADER_COMPILE_ERROR,
	GLSL_TESS_EVALUATION_SHADER_COMPILE_ERROR,
	GLSL_GEOMETRY_SHADER_COMPILE_ERROR,
	GLSL_FRAGMENT_SHADER_COMPILE_ERROR,
	GLSL_COMPUTE_SHADER_COMPILE_ERROR,

	GLSL_PROGRAM_OBJECT_CREATION_ERROR,
	GLSL_PROGRAM_OBJECT_LINK_ERROR

};

class StringUtility {

public:

	enum class StringResult : uint8_t { S_OKAY, S_FILE_NOT_FOUND, S_BUFFER_ALLOC_ERROR };

	// Split a path into it's component parts - separated by characters in delimiters, and return a vector of the component parts
	static std::vector<std::string> splitPath(const std::string& path, const std::set<char>& delimiters);

	static std::string loadStringFromFile(const std::string& filePath);
};


// Basic shader object creation function takes a path to a vertex shader file and fragment shader file and returns a bound and linked shader program object
GLuint setupShaders(const std::string& vsPath,
	const std::string& fsPath,
	ShaderError* error_result = NULL);
