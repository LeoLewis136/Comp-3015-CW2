#include "ShaderManager.h"

// Function to create and setup a shader
ShaderManager::ShaderManager(const char* fragName, const char* vertName) {
	prog.compileShader(fragName, GLSLShader::FRAGMENT); // Fragment shader
	prog.compileShader(vertName, GLSLShader::VERTEX); // Vertex shader
	prog.link(); // Link the shaders
}

// Function to switch to a different shader
void ShaderManager::switchShader() {
	prog.use();
}

void ShaderManager::setFloat(const char* uniformName, float toSet) {
	prog.setUniform(uniformName, toSet);
}
void ShaderManager::setMat4(const char* uniformName, glm::mat4 toSet) {
	prog.setUniform(uniformName, toSet);
}