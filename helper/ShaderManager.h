#pragma once

#include "glslprogram.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Shader manager class to wrap the GLSLProgram and give simpler controls 
class ShaderManager {
private:
	GLSLProgram prog; // The actual glslprogram object that stores everything required

public:
	ShaderManager(const char* fragName, const char* vertName);

	void switchShader();

	void setFloat(const char *uniformName, float toSet); // Setting a float uniform
	void setMat4(const char* uniformName, glm::mat4 toSet); // Setting a mat4 uniform
};