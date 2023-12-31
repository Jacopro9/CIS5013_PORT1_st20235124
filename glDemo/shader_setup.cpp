
#include "shader_setup.h"

using namespace std;

#pragma region StringUtility implementation

vector<string> StringUtility::splitPath(const string& path, const set<char>& delimiters) {

	vector<string> result;

	char const* charPtr = path.c_str();
	char const* startChar = charPtr;

	while (*charPtr) {

		if (delimiters.find(*charPtr) != delimiters.end())
		{
			if (startChar != charPtr) {

				string pathComponent(startChar, charPtr);
				result.push_back(pathComponent);
			}
			else {

				result.push_back("");
			}

			startChar = charPtr + 1;
		}

		++charPtr;
	}

	result.push_back(startChar);

	return result;
}


string StringUtility::loadStringFromFile(const string& filePath) {

	ifstream shaderFile(filePath);

	if (!shaderFile.is_open()) {

		throw StringUtility::StringResult::S_FILE_NOT_FOUND;
	}

	// Get file size and allocate buffer
	streampos startIndex = shaderFile.tellg();
	shaderFile.seekg(0, ios::end);
	streampos endIndex = shaderFile.tellg();

	auto bufferSize = endIndex - startIndex;

	char* src = (char*)calloc(bufferSize + 1, 1); // Ensure buffer ends will null terminator character

	if (!src) {

		shaderFile.close();
		throw StringUtility::StringResult::S_BUFFER_ALLOC_ERROR;
	}

	shaderFile.seekg(0, ios::beg);
	shaderFile.read(src, bufferSize); // Read file into c string

	string sourceString(src);

	free(src);
	shaderFile.close();

	return sourceString;
}

#pragma endregion


// private function declarations for shader loader

static ShaderError createShaderFromFile(GLenum shaderType, const string& shaderFilePath, GLuint* shaderObject);
static const string* loadShaderSourceStringFromFile(const string& filePath);
static void printSourceListing(const string& sourceString, bool showLineNumbers = true);
static void reportProgramInfoLog(GLuint program);
static void reportShaderInfoLog(GLuint shader);


// Structure to track the loading of individual shader objects
struct ShaderBuildInfo {

	GLuint			vertexShader = 0;
	GLuint			tessControlShader = 0;
	GLuint			tessEvaluationShader = 0;
	GLuint			geometryShader = 0;
	GLuint			fragmentShader = 0;

public:

	ShaderBuildInfo() {}

	~ShaderBuildInfo() {

		if (vertexShader)
			glDeleteShader(vertexShader);

		if (tessControlShader)
			glDeleteShader(tessControlShader);

		if (tessEvaluationShader)
			glDeleteShader(tessEvaluationShader);

		if (geometryShader)
			glDeleteShader(geometryShader);

		if (fragmentShader)
			glDeleteShader(fragmentShader);
	}
};


// Structure to contain load and seutp info for each shader type - not used beyond setup process
struct ShaderType {

	GLenum shaderType;
	string capName;
	string name;
	ShaderError errSourceNotFound;
	ShaderError errShaderObjectCreation;
	ShaderError errShaderCompilation;

	ShaderType() {}
	ShaderType(GLenum shaderType, const char* capName, const char* name, ShaderError errSourceNotFound, ShaderError errShaderObjectCreation, ShaderError errShaderCompilation) {

		this->shaderType = shaderType;
		this->capName = string(capName);
		this->name = string(name);
		this->errSourceNotFound = errSourceNotFound;
		this->errShaderObjectCreation = errShaderObjectCreation;
		this->errShaderCompilation = errShaderCompilation;
	}
};


static map<GLenum, ShaderType> loader_info = {

	{GL_VERTEX_SHADER, ShaderType(GL_VERTEX_SHADER, "Vertex", "vertex", ShaderError::GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND, ShaderError::GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR, ShaderError::GLSL_VERTEX_SHADER_COMPILE_ERROR)},

	{GL_TESS_CONTROL_SHADER, ShaderType(GL_TESS_CONTROL_SHADER, "Tessellation Control", "tessellation control", ShaderError::GLSL_TESS_CONTROL_SHADER_SOURCE_NOT_FOUND, ShaderError::GLSL_TESS_CONTROL_SHADER_OBJECT_CREATION_ERROR, ShaderError::GLSL_TESS_CONTROL_SHADER_COMPILE_ERROR)},

	{GL_TESS_EVALUATION_SHADER, ShaderType(GL_TESS_EVALUATION_SHADER, "Tessellation Evaluation", "tessellation evaluation", ShaderError::GLSL_TESS_EVALUATION_SHADER_SOURCE_NOT_FOUND, ShaderError::GLSL_TESS_EVALUATION_SHADER_OBJECT_CREATION_ERROR, ShaderError::GLSL_TESS_EVALUATION_SHADER_COMPILE_ERROR)},

	{GL_GEOMETRY_SHADER, ShaderType(GL_GEOMETRY_SHADER, "Geometry", "geometry", ShaderError::GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND, ShaderError::GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR, ShaderError::GLSL_GEOMETRY_SHADER_COMPILE_ERROR)},

	{GL_FRAGMENT_SHADER, ShaderType(GL_FRAGMENT_SHADER, "Fragment", "fragment", ShaderError::GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND, ShaderError::GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR, ShaderError::GLSL_FRAGMENT_SHADER_COMPILE_ERROR)}
};



//GLuint setupShaders(const string& vsPath, const string& gsPath, const string& tessControlPath, const string& tessEvaluationPath, const string& fsPath, ShaderError* error_result) {
GLuint setupShaders(const string& vsPath, const string& fsPath, ShaderError* error_result) {

	ShaderBuildInfo buildInfo;

	// Load vertex shader
	ShaderError err = createShaderFromFile(GL_VERTEX_SHADER, vsPath, &(buildInfo.vertexShader));

	if (err != ShaderError::GLSL_OK) {

		if (error_result)
			*error_result = err;

		return 0;
	}

#if 0
	// Load tessellation shader(s) (control optional)
	if (tessControlPath.length() > 0) {

		err = createShaderFromFile(GL_TESS_CONTROL_SHADER, tessControlPath, &(buildInfo.tessControlShader));

		if (err != ShaderError::GLSL_OK) {

			if (error_result)
				*error_result = err;

			return 0;
		}
	}

	if (tessEvaluationPath.length() > 0) {

		err = createShaderFromFile(GL_TESS_EVALUATION_SHADER, tessEvaluationPath, &(buildInfo.tessEvaluationShader));

		if (err != ShaderError::GLSL_OK) {

			if (error_result)
				*error_result = err;

			return 0;
		}
	}

	// Load geometry shader (if needed)
	if (gsPath.length() > 0) {

		err = createShaderFromFile(GL_GEOMETRY_SHADER, gsPath, &(buildInfo.geometryShader));

		if (err != ShaderError::GLSL_OK) {

			if (error_result)
				*error_result = err;

			return 0;
		}
	}

#endif

	// Load fragment shader
	err = createShaderFromFile(GL_FRAGMENT_SHADER, fsPath, &(buildInfo.fragmentShader));

	if (err != ShaderError::GLSL_OK) {

		if (error_result)
			*error_result = err;

		return 0;
	}


	// Once shader objects have been validated, setup the main shader program object
	GLuint program = glCreateProgram();

	if (program == 0) {

		cout << "The shader program object could not be created." << endl;

		if (error_result)
			*error_result = ShaderError::GLSL_PROGRAM_OBJECT_CREATION_ERROR;

		return 0;
	}


	// Attach shader objects
	if (buildInfo.vertexShader != 0)
		glAttachShader(program, buildInfo.vertexShader);

	if (buildInfo.tessControlShader != 0)
		glAttachShader(program, buildInfo.tessControlShader);

	if (buildInfo.tessEvaluationShader != 0)
		glAttachShader(program, buildInfo.tessEvaluationShader);

	if (buildInfo.geometryShader != 0)
		glAttachShader(program, buildInfo.geometryShader);

	if (buildInfo.fragmentShader != 0)
		glAttachShader(program, buildInfo.fragmentShader);


	// Link and validate the shader program
	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {

		// Failed to link - report linker error log and dispose of local resources

		cout << "The shader program object could not be linked successfully..." << endl;

		cout << "\n<GLSL shader program object linker errors--------------------->\n\n";
		reportProgramInfoLog(program);
		cout << "<-----------------end shader program object linker errors>\n\n";

		glDeleteProgram(program);

		if (error_result)
			*error_result = ShaderError::GLSL_PROGRAM_OBJECT_LINK_ERROR;

		return 0;
	}

	// Shader program object setup successfully

	if (error_result)
		*error_result = ShaderError::GLSL_OK;

	return program;
}

#if 0
GLuint setupComputeShader(const std::string& csPath, GLSL_ERROR* error_result) {

	GLuint					computeShader = 0;
	GLuint					glslProgram = 0;

	const string* computeShaderSource = NULL;

	// create compute shader object
	GLSL_ERROR err = createShaderFromFile(GL_COMPUTE_SHADER, csPath, &computeShader, &computeShaderSource);

	if (err != GLSL_OK) {

		switch (err) {

		case GLSL_SHADER_SOURCE_NOT_FOUND:

			printf("Compute shader source not found.\n");

			if (error_result)
				*error_result = GLSL_COMPUTE_SHADER_SOURCE_NOT_FOUND;

			return 0;


		case GLSL_SHADER_OBJECT_CREATION_ERROR:

			printf("OpenGL could not create the compute shader program object.  Try using fewer resources before creating the program object.\n");

			if (error_result)
				*error_result = GLSL_COMPUTE_SHADER_OBJECT_CREATION_ERROR;

			return 0;


		case GLSL_SHADER_COMPILE_ERROR:

			printf("The compute shader could not be compiled successfully...\n");
			printf("Compute shader source code...\n\n");

			printSourceListing(*computeShaderSource);

			// report compilation error log

			printf("\n<compute shader compiler errors--------------------->\n\n");
			reportShaderInfoLog(computeShader);
			printf("<-----------------end compute shader compiler errors>\n\n\n");

			// dispose of existing shader objects

			glDeleteShader(computeShader);

			if (computeShaderSource)
				delete computeShaderSource;

			if (error_result)
				*error_result = GLSL_COMPUTE_SHADER_COMPILE_ERROR;

			return 0;


		default:

			printf("The compute shader object could not be created successfully.\n");

			// dispose of existing shader objects

			glDeleteShader(computeShader);

			if (computeShaderSource)
				delete computeShaderSource;

			if (error_result)
				*error_result = err;

			return 0;
		}
	}



	// source code objects not longer needed
	if (computeShaderSource)
		delete computeShaderSource;

	//
	// Once the compute shader object has been validated, setup the main
	// shader program object
	//
	glslProgram = glCreateProgram();

	if (!glslProgram) {

		printf("The shader program object could not be created.\n");

		glDeleteShader(computeShader);

		if (error_result)
			*error_result = GLSL_PROGRAM_OBJECT_CREATION_ERROR;

		return 0;
	}


	// Attach shader objects
	glAttachShader(glslProgram, computeShader);

	// link the shader program
	glLinkProgram(glslProgram);


	// validate link status
	GLint linkStatus;

	glGetProgramiv(glslProgram, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {

		// failed to link - report linker error log and dispose of local resources

		printf("The shader program object could not be linked successfully...\n");

		// report linker error log

		printf("\n<GLSL shader program object linker errors--------------------->\n\n");
		reportProgramInfoLog(glslProgram);
		printf("<-----------------end shader program object linker errors>\n\n");

		// delete program and detach shaders
		glDeleteProgram(glslProgram);

		// delete shaders
		glDeleteShader(computeShader);

		if (error_result)
			*error_result = GLSL_PROGRAM_OBJECT_LINK_ERROR;

		return 0;
	}

	// cleanup - delete individual shader objects since they're now included as part of the overall shader program object
	glDeleteShader(computeShader);

	if (error_result)
		*error_result = GLSL_OK;

	return glslProgram;
}
#endif


//
// private function implementation
//


ShaderError createShaderFromFile(GLenum shaderType, const string& shaderFilePath, GLuint* shaderObject) {

	GLuint shader = 0;
	string sourceString;

	try {

		sourceString = StringUtility::loadStringFromFile(shaderFilePath);

		shader = glCreateShader(shaderType);

		if (shader == 0)
			throw ShaderError::GLSL_SHADER_OBJECT_CREATION_ERROR;

		const char* src = sourceString.c_str();
		glShaderSource(shader, 1, static_cast<const GLchar**>(&src), 0);

		glCompileShader(shader);

		GLint compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == 0)
			throw ShaderError::GLSL_SHADER_COMPILE_ERROR;

		*shaderObject = shader;

		return ShaderError::GLSL_OK;
	}
	catch (StringUtility::StringResult err) {

		set<char> pathDelimiters{ '\\' };
		vector<string> pathComponents = StringUtility::splitPath(shaderFilePath, pathDelimiters);

		if (err == StringUtility::StringResult::S_FILE_NOT_FOUND) {

			cout << (pathComponents[pathComponents.size() - 1]) << " source not found. Check the file path in your code.\n";
		}

		return ShaderError::GLSL_SHADER_SOURCE_NOT_FOUND;
	}
	catch (ShaderError err) {

		set<char> pathDelimiters{ '\\' };
		vector<string> pathComponents = StringUtility::splitPath(shaderFilePath, pathDelimiters);

		if (err == ShaderError::GLSL_SHADER_OBJECT_CREATION_ERROR) {

			cout << (pathComponents[pathComponents.size() - 1]) << " shader object could not be created.  Try freeing up resources before attempting to create the shader.\n";

			return err;
		}
		else if (err == ShaderError::GLSL_SHADER_COMPILE_ERROR) {

			set<char> pathDelimiters{ '\\' };
			vector<string> pathComponents = StringUtility::splitPath(shaderFilePath, pathDelimiters);

			cout << pathComponents[pathComponents.size() - 1] << " could not be compiled successfully...\n\n";
			printSourceListing(sourceString);

			// report compilation error log

			cout << "\n<" << pathComponents[pathComponents.size() - 1] << " shader compiler errors--------------------->\n\n";
			reportShaderInfoLog(shader);
			cout << "<-----------------end " << pathComponents[pathComponents.size() - 1] << " shader compiler errors>\n\n\n";

			glDeleteShader(shader);
			shader = 0;

			return err;
		}
	}
}


const string* loadShaderSourceStringFromFile(const string& filePath) {

	string* sourceString = NULL;

	const char* file_str = filePath.c_str();

	struct stat fileStatus;
	int file_error = stat(file_str, &fileStatus);

	if (file_error == 0) {

		_off_t fileSize = fileStatus.st_size;

		char* src = (char*)calloc(fileSize + 1, 1); // add null-terminator character at end of string

		if (src) {

			ifstream shaderFile(file_str);

			if (shaderFile.is_open()) {

				shaderFile.read(src, fileSize);
				sourceString = new string(src);

				shaderFile.close();
			}

			// dispose of local resources
			free(src);
		}
	}

	// return pointer to new source string
	return sourceString;
}


void printSourceListing(const string& sourceString, bool showLineNumbers) {

	const char* srcPtr = sourceString.c_str();
	const char* srcEnd = srcPtr + sourceString.length();

	size_t lineIndex = 0;

	while (srcPtr < srcEnd) {

		if (showLineNumbers) {

			cout.fill(' ');
			cout.width(4);
			cout << dec << ++lineIndex << " > ";
		}

		size_t substrLength = strcspn(srcPtr, "\n");

		cout << string(srcPtr, 0, substrLength) << endl;

		srcPtr += substrLength + 1;
	}
}


void reportProgramInfoLog(GLuint program)
{
	GLsizei			noofBytes = 0;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &noofBytes);

	GLchar* str = (GLchar*)calloc(noofBytes + 1, 1);

	if (str) {

		glGetProgramInfoLog(program, noofBytes, 0, str);

		cout << str << endl;
		//printf("%s\n", str);

		free(str);
	}
}


void reportShaderInfoLog(GLuint shader)
{
	GLsizei			noofBytes = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &noofBytes);

	GLchar* str = (GLchar*)calloc(noofBytes + 1, 1);

	if (str) {

		glGetShaderInfoLog(shader, noofBytes, 0, str);

		cout << str << endl;
		//printf("%s\n", str);

		free(str);
	}
}
