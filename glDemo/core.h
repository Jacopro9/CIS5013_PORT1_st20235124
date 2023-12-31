#pragma once

// These libraries are needed to link the program (Visual Studio specific)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"lib\\glfw3.lib")
#pragma comment(lib,"lib\\glew32s.lib")
#pragma comment(lib,"lib\\glew32.lib")
#pragma comment(lib,"lib\\FreeImage.lib")
#pragma comment(lib,"lib\\assimp-vc143-mt.lib")

#define GLEW_STATIC
#include "GL/glew.h" 
#include "GLFW/glfw3.h"

#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat3x3.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <FreeImage\FreeImage.h>
#include <assimp\cimport.h>			// Main C import interface
#include <assimp\scene.h>			// Output data structure
#include <assimp\postprocess.h>		// Post processing flags