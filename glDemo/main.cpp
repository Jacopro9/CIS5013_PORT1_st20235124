#include "core.h"
#include "TextureLoader.h"
#include "shader_setup.h"
#include "ArcballCamera.h"
#include "GUClock.h"
#include "AIMesh.h"


using namespace std;
using namespace glm;


struct DirectionalLight {

	vec3 direction;
	vec3 colour;
	
	DirectionalLight() {

		direction = vec3(0.0f, 1.0f, 0.0f); // default to point upwards
		colour = vec3(1.0f, 1.0f, 1.0f);
	}

	DirectionalLight(vec3 direction, vec3 colour = vec3(1.0f, 1.0f, 1.0f)) {

		this->direction = direction;
		this->colour = colour;
	}
};

struct PointLight {

	vec3 pos;
	vec3 colour;
	vec3 attenuation; // x=constant, y=linear, z=quadratic

	PointLight() {

		pos = vec3(0.0f, 0.0f, 0.0f);
		colour = vec3(1.0f, 1.0f, 1.0f);
		attenuation = vec3(1.0f, 1.0f, 1.0f);
	}

	PointLight(vec3 pos, vec3 colour = vec3(1.0f, 1.0f, 1.0f), vec3 attenuation = vec3(1.0f, 1.0f, 1.0f)) {

		this->pos = pos;
		this->colour = colour;
		this->attenuation = attenuation;
	}
};


#pragma region Global variables

// Window size
unsigned int		windowWidth = 1024;
unsigned int		windowHeight = 768;

// Main clock for tracking time (for animation / interaction)
GUClock*			gameClock = nullptr;

// Main camera
ArcballCamera*		mainCamera = nullptr;

// Mouse tracking
bool				mouseDown = false;
double				prevMouseX, prevMouseY;

// Keyboard tracking
bool				forwardPressed;
bool				backPressed;
bool				leftPressed;
bool				rightPressed;


// Scene objects
AIMesh*				terrainMesh = nullptr;
AIMesh*				waterMesh = nullptr;

// multi-mesh models
vector<AIMesh*> tier1Model = vector<AIMesh*>();
vector<AIMesh*> tier2Model = vector<AIMesh*>();
vector<AIMesh*> tier3Model = vector<AIMesh*>();
vector<AIMesh*> robot = vector<AIMesh*>();

// Shaders

// Basic colour shader
GLuint				basicShader;
GLint				basicShader_mvpMatrix;

//  *** normal mapping *** Normal mapped texture with Directional light
// This is the same as the texture direct light shader above, but with the addtional uniform variable
// to set the normal map sampler2D variable in the fragment shader.
GLuint				nMapDirLightShader;
GLint				nMapDirLightShader_modelMatrix;
GLint				nMapDirLightShader_viewMatrix;
GLint				nMapDirLightShader_projMatrix;
GLint				nMapDirLightShader_diffuseTexture;
GLint				nMapDirLightShader_normalMapTexture;
GLint				nMapDirLightShader_lightDirection;
GLint				nMapDirLightShader_lightColour;

// cylinder model
vec3 cylinderPos = vec3(-2.0f, 2.0f, 0.0f);

// camera model
vec3 cameraPos = vec3(2.0f, 0.0f, 0.0f);

// Directional light example (declared as a single instance)
float directLightTheta = glm::radians(70.0f);
float directLightTheta2 = glm::radians(25.0f);
float directLightTheta3 = glm::radians(165.0f);
DirectionalLight directLight = DirectionalLight(vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f), vec3(1.0f, 1.0f, 1.0f));
DirectionalLight directLightBlue = DirectionalLight(vec3(cosf(directLightTheta2), sinf(directLightTheta2), 0.0f), vec3(0.0f, 0.0f, 1.0f));
DirectionalLight directLightPink = DirectionalLight(vec3(cosf(directLightTheta3), sinf(directLightTheta3), 0.0f), vec3(1.0f, 0.0f, 0.0f));

// Setup point light example light (use array to make adding other lights easier later)
PointLight lights[1] = {
	PointLight(vec3(3.5f, 0.4f, 3.5f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.1f, 0.001f))
};

bool rotateDirectionalLight = false;

#pragma endregion


// Function prototypes
void renderScene();
void renderWithMultipleLights();
void renderWithTransparency();
void updateScene();
void resizeWindow(GLFWwindow* window, int width, int height);
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos);
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods);
void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset);
void mouseEnterHandler(GLFWwindow* window, int entered);

vector<AIMesh*> multiMesh(string objectFile, string diffuseMapFile, string normalMapFile)
{
	vector<AIMesh*> model;
	const struct aiScene* modelScene = aiImportFile(objectFile.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (modelScene) {

		cout << "Model: " << objectFile << " has " << modelScene->mNumMeshes << " meshe(s)\n";

		if (modelScene->mNumMeshes > 0) {
			GLuint texture = loadTexture(diffuseMapFile.c_str(), FIF_BMP);
			GLuint normapMap = loadTexture(normalMapFile.c_str(), FIF_BMP);
			// For each sub-mesh, setup a new AIMesh instance in the houseModel array
			for (int i = 0; i < modelScene->mNumMeshes; i++) {

				cout << "Loading model sub-mesh " << i << endl;
				model.push_back(new AIMesh(modelScene, i));
				model[i]->addTexture(texture);
				model[i]->addNormalMap(normapMap);
			}
			return model;
		}
	}
	else cout << objectFile;
}

int main() {

	// 1. Initialisation

	gameClock = new GUClock();

#pragma region OpenGL and window setup

	// Initialise glfw and setup window
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "CIS5013", NULL, NULL);

	// Check window was created successfully
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set callback functions to handle different events
	glfwSetFramebufferSizeCallback(window, resizeWindow); // resize window callback
	glfwSetKeyCallback(window, keyboardHandler); // Keyboard input callback
	glfwSetCursorPosCallback(window, mouseMoveHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetScrollCallback(window, mouseScrollHandler);
	glfwSetCursorEnterCallback(window, mouseEnterHandler);

	// Initialise glew
	glewInit();
	
	// Setup window's initial size
	resizeWindow(window, windowWidth, windowHeight);

#pragma endregion

	// Initialise scene - geometry and shaders etc
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // setup background colour to be black
	glClearDepth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// Setup Textures, VBOs and other scene objects

	mainCamera = new ArcballCamera(-33.0f, 45.0f, 40.0f, 55.0f, (float)windowWidth/(float)windowHeight, 0.1f, 5000.0f);
	
	terrainMesh = new AIMesh(string("Assets\\terrain\\terrain.obj"));
	if (terrainMesh) {
		terrainMesh->addTexture(string("Assets\\terrain\\sand_c.bmp"), FIF_BMP);
		terrainMesh->addNormalMap(string("Assets\\terrain\\sand_n.bmp"), FIF_BMP);
	}

	waterMesh = new AIMesh(string("Assets\\terrain\\water.obj"));
	if (waterMesh) {
		waterMesh->addTexture(string("Assets\\terrain\\water.bmp"), FIF_BMP);
		waterMesh->addNormalMap(string("Assets\\terrain\\water_n.bmp"), FIF_BMP);
	}

	// Load shaders
	basicShader = setupShaders(string("Assets\\Shaders\\basic_shader.vert"), string("Assets\\Shaders\\basic_shader.frag"));
	nMapDirLightShader = setupShaders(string("Assets\\Shaders\\nmap-directional.vert"), string("Assets\\Shaders\\nmap-directional.frag"));

	// Get uniform variable locations for setting values later during rendering
	basicShader_mvpMatrix = glGetUniformLocation(basicShader, "mvpMatrix");

	nMapDirLightShader_modelMatrix = glGetUniformLocation(nMapDirLightShader, "modelMatrix");
	nMapDirLightShader_viewMatrix = glGetUniformLocation(nMapDirLightShader, "viewMatrix");
	nMapDirLightShader_projMatrix = glGetUniformLocation(nMapDirLightShader, "projMatrix");
	nMapDirLightShader_diffuseTexture = glGetUniformLocation(nMapDirLightShader, "diffuseTexture");
	nMapDirLightShader_normalMapTexture = glGetUniformLocation(nMapDirLightShader, "normalMapTexture");
	nMapDirLightShader_lightDirection = glGetUniformLocation(nMapDirLightShader, "lightDirection");
	nMapDirLightShader_lightColour = glGetUniformLocation(nMapDirLightShader, "lightColour");

	// calling multimesh function to import the models
	tier1Model = multiMesh(string("Assets\\buildings\\tier1.v2.obj"), string("Assets\\buildings\\house_c3.bmp"), string("Assets\\buildings\\house_n3.bmp"));
	tier2Model = multiMesh(string("Assets\\buildings\\tier2.v2.obj"), string("Assets\\buildings\\house_c3.bmp"), string("Assets\\buildings\\house_n3.bmp"));
	tier3Model = multiMesh(string("Assets\\buildings\\tier3.obj"), string("Assets\\buildings\\house_c3.bmp"), string("Assets\\buildings\\house_n3.bmp"));
	
	robot = multiMesh(string("Assets\\robot\\robototo1.obj"), string("Assets\\robot\\robot_c.bmp"), string("Assets\\robot\\robot_n.bmp"));
	

	// 2. Main loop

	while (!glfwWindowShouldClose(window)) {

		updateScene();
		renderScene();					// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		glfwPollEvents();				// Use this version when animating as fast as possible
	
		// update window title
		char timingString[256];
		sprintf_s(timingString, 256, "CIS5013: Average fps: %.0f; Average spf: %f", gameClock->averageFPS(), gameClock->averageSPF() / 1000.0f);
		glfwSetWindowTitle(window, timingString);
	}

	glfwTerminate();

	if (gameClock) {

		gameClock->stop();
		gameClock->reportTimingData();
	}

	return 0;
}


// renderScene - function to render the current scene
void renderScene()
{
	renderWithMultipleLights();
	//renderWithTransparency();
}

// Demonstrate the use of a single directional light source
//  *** normal mapping ***  - since we're demonstrating the use of normal mapping with a directional light,
// the normal mapped objects are rendered here also!
void renderWithTransparency() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -cameraPos);

#pragma region Render opaque objects with directional light

	//  *** normal mapping ***
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	if (terrainMesh) {

		mat4 modelTransform = glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		terrainMesh->setupTextures();
		terrainMesh->render();
	}
	if (!tier1Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-0.5f, 0.6f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		// Loop through array of meshes and render each one
		for (AIMesh* mesh : tier1Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier2Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.0f, 0.3f, -1.0f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier2Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier3Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.0f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier3Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!robot.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.4f, 3.5f)) * glm::scale(identity<mat4>(), vec3(0.03f, 0.03f, 0.03f)) * eulerAngleY<float>(glm::radians(270.0f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : robot) {

			mesh->setupTextures();
			mesh->render();
		}
	}
#pragma endregion

#pragma region Render transparant objects

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	if (waterMesh) {

		mat4 modelTransform = glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		waterMesh->setupTextures();
		waterMesh->render();
	}

	glDisable(GL_BLEND);

#pragma endregion

	// render directional light source

	// Restore fixed-function pipeline
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glBegin(GL_POINTS);

	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x * 10.0f, directLight.direction.y * 10.0f, directLight.direction.z * 10.0f);

	glEnd();
}


// Demonstrate the use of a multiple coloured directional light sources
// also uses normal mapping
void renderWithMultipleLights() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -cameraPos);

#pragma region Render opaque objects with directional light

	//  *** normal mapping ***
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	if (terrainMesh) {

		mat4 modelTransform = glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		terrainMesh->setupTextures();
		terrainMesh->render();
	}

	// Render models
	if (!tier1Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-0.5f, 0.6f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		// Loop through array of meshes and render each one
		for (AIMesh* mesh : tier1Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier2Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.0f, 0.3f, -1.0f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier2Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier3Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.0f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier3Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!robot.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.4f, 3.5f)) * glm::scale(identity<mat4>(), vec3(0.03f, 0.03f, 0.03f)) * eulerAngleY<float>(glm::radians(270.0f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : robot) {

			mesh->setupTextures();
			mesh->render();
		}
	}
#pragma endregion
	// Enable additive blending for ***subsequent*** light sources!!!
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

#pragma region Render opaque objects with 2nd directional light

	//  *** normal mapping ***
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLightPink.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLightPink.colour));

	if (terrainMesh) {

		mat4 modelTransform = glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		terrainMesh->setupTextures();
		terrainMesh->render();
	}

	// Render models
	if (!tier1Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-0.5f, 0.6f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		// Loop through array of meshes and render each one
		for (AIMesh* mesh : tier1Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier2Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.0f, 0.3f, -1.0f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier2Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!tier3Model.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.0f, 1.5f)) * glm::scale(identity<mat4>(), vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : tier3Model) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	if (!robot.empty()) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(3.5f, 0.4f, 3.5f)) * glm::scale(identity<mat4>(), vec3(0.03f, 0.03f, 0.03f)) * eulerAngleY<float>(glm::radians(270.0f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : robot) {

			mesh->setupTextures();
			mesh->render();
		}
	}
	glDisable(GL_BLEND);
#pragma endregion

	// Restore fixed-function pipeline
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glBegin(GL_POINTS);

	glColor3f(directLightPink.colour.r, directLightPink.colour.g, directLightPink.colour.b);
	glVertex3f(directLightPink.direction.x * 10.0f, directLightPink.direction.y * 10.0f, directLightPink.direction.z * 10.0f);

	glColor3f(directLightBlue.colour.r, directLightBlue.colour.g, directLightBlue.colour.b);
	glVertex3f(directLightBlue.direction.x * 10.0f, directLightBlue.direction.y * 10.0f, directLightBlue.direction.z * 10.0f);

	glEnd();
}

// Function called to animate elements in the scene
void updateScene() {

	float tDelta = 0.0f;

	if (gameClock) {

		gameClock->tick();
		tDelta = (float)gameClock->gameTimeDelta();
	}

	// update main light source
	if (rotateDirectionalLight) {

		directLightTheta += glm::radians(30.0f) * tDelta;
		directLight.direction = vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f);
	}

	// Handle movement based on user input

	float moveSpeed = 3.0f; // movement displacement per second
	float rotateSpeed = 90.0f; // degrees rotation per second

	if (forwardPressed) {
		float dPos = -moveSpeed * tDelta; // calc movement based on time elapsed
		cameraPos += vec3( dPos, 0, dPos); // add displacement to position vector
	}
	else if (backPressed) {
		float dPos = moveSpeed * tDelta; // calc movement based on time elapsed
		cameraPos += vec3(dPos, 0, dPos); // add displacement to position vector
	}

	if (leftPressed) {
		float dPos = -moveSpeed * tDelta; // calc movement based on time elapsed
		cameraPos += vec3(dPos, 0, -dPos); // add displacement to position vector
	}
	else if (rightPressed) {
		float dPos = -moveSpeed * tDelta; // calc movement based on time elapsed
		cameraPos += vec3(-dPos, 0, dPos); // add displacement to position vector
	}

}


#pragma region Event handler functions

// Function to call when window resized
void resizeWindow(GLFWwindow* window, int width, int height)
{
	if (mainCamera) {

		mainCamera->setAspect((float)width / (float)height);
	}

	// Update viewport to cover the entire window
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;
}

// Function to call to handle keyboard input
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {

		// check which key was pressed...
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			
			case GLFW_KEY_W:
				forwardPressed = true;
				break;

			case GLFW_KEY_S:
				backPressed = true;
				break;

			case GLFW_KEY_A:
				leftPressed = true;
				break;

			case GLFW_KEY_D:
				rightPressed = true;
				break;

			case GLFW_KEY_SPACE:
				rotateDirectionalLight = !rotateDirectionalLight;
				break;

			default:
			{
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		// handle key release events
		switch (key)
		{
			case GLFW_KEY_W:
				forwardPressed = false;
				break;

			case GLFW_KEY_S:
				backPressed = false;
				break;

			case GLFW_KEY_A:
				leftPressed = false;
				break;

			case GLFW_KEY_D:
				rightPressed = false;
				break;

			default:
			{
			}
		}
	}
}


void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos) {

	if (mouseDown) {

		float dx = float(xpos - prevMouseX);
		float dy = float(ypos - prevMouseY);

		if (mainCamera)
			mainCamera->rotateCamera(-dy, -dx);

		prevMouseX = xpos;
		prevMouseY = ypos;
	}

}

void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods) {

	/*if (button == GLFW_MOUSE_BUTTON_LEFT) {

		if (action == GLFW_PRESS) {

			mouseDown = true;
			glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
		}
		else if (action == GLFW_RELEASE) {

			mouseDown = false;
		}
	}*/
}

void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {

	if (mainCamera) {

		if (yoffset < 0.0)
			mainCamera->scaleRadius(1.1f);
		else if (yoffset > 0.0)
			mainCamera->scaleRadius(0.9f);
	}
}

void mouseEnterHandler(GLFWwindow* window, int entered) {
}

#pragma endregion