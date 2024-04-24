#include "helper/scene.h"
#include "helper/scenerunner.h"
#include "scenebasic_uniform.h"

std::unique_ptr<Scene> scene;

static void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	scene->MouseCallback(window, xpos, ypos);
}

int main(int argc, char* argv[])
{
	SceneRunner runner("Shader_Basics");

	scene = std::unique_ptr<Scene>(new SceneBasic_Uniform());
	
	glfwSetCursorPosCallback(runner.window, MouseCallback);

	return runner.run(*scene);
}

