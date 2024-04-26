#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

// Helper includes
#include "helper/glutils.h"
#include "helper/texture.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float3.hpp>

using glm::vec3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : sky(200) , water(60, 60, 1, 1), playerObject() {}

float SceneBasic_Uniform::gauss(float x, float sigma2) {
	double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
	double exponent = -(-x * x) / (2.0 * sigma2);
	return (float)(coeff * exp(exponent));
}

void SceneBasic_Uniform::initScene()
{
	compile();

	model = mat4(1.0f);

	// Postion, look position, up direction
	view = glm::lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	standardShaders.use();

	standardShaders.setUniform("BloomThreshold", 0.8f); 
	standardShaders.setUniform("BloomIntensity", 1.0f);


	// Gaussian weight setup
	float weights[5], sum, sigma2 = 8.0f;

	weights[0] = gauss(0, sigma2);
	sum = weights[0];
	for (int i = 1; i < 5; i++) {
		weights[i] = gauss(float(i), sigma2);
		sum += 2 * weights[i];
	}

	for (int i = 1; i < 5; i++) {
		std::stringstream uniName;
		uniName << "Weights[" << i << "]";
		float val = weights[i] / sum;
		standardShaders.setUniform(uniName.str().c_str(), val);
	}

	skyboxShaders.use();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 

	// -- Load Textures -- 
	skyboxTexture = Texture::loadCubeMap("media/texture/Skybox/vz_sinister", ".png"); 
	plainTexture = Texture::loadTexture("media/texture/MainTexture.png");

	// -- Load Objects --
	tempTesting = ObjMesh::load("media/Ship.obj", true);

	setupFBO();

	GLfloat verts[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};

	GLfloat tc[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	// Multipass VAO setup
	unsigned int handle[2];
	glGenBuffers(2, handle);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

	glGenVertexArrays(1, &fsQuad);
	glBindVertexArray(fsQuad);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2); 
	
	glBindVertexArray(0);

	
}


void SceneBasic_Uniform::setupFBO() {
	glGenFramebuffers(1, &renderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	GLuint depthBuf; 
	glGenRenderbuffers(1, &depthBuf); 
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height); 

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf); 
	
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	// Stick debug code
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is complete" << endl;
	}
	else {
		std::cout << "Framebuffer error:" << result << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &verticalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, verticalFBO);

	glGenTextures(1, &verticalTex);
	glBindTexture(GL_TEXTURE_2D, verticalTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, verticalTex, 0);

	glDrawBuffers(1, drawBuffers);
	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is complete" << endl;
	}
	else {
		std::cout << "Framebuffer error:" << result << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &horizontalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, horizontalFBO);

	glGenTextures(1, &horizontalTex);
	glBindTexture(GL_TEXTURE_2D, horizontalTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, horizontalTex, 0);

	glDrawBuffers(1, drawBuffers);
	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is complete" << endl;
	}
	else {
		std::cout << "Framebuffer error:" << result << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

	glGenTextures(1, &lightTex);
	glBindTexture(GL_TEXTURE_2D, lightTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTex, 0);

	glDrawBuffers(1, drawBuffers);
	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is complete" << endl;
	}
	else {
		std::cout << "Framebuffer error:" << result << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::compile()
{

	// Compiling a standard shader used for basic lit 3d objects
	try {
		standardShaders.compileShader("shader/basic_uniform.vert");
		standardShaders.compileShader("shader/basic_uniform.frag");
		standardShaders.link();


		skyboxShaders.compileShader("shader/skybox.vert");
		skyboxShaders.compileShader("shader/skybox.frag");
		skyboxShaders.link();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}


void SceneBasic_Uniform::setMatricies(GLSLProgram& currentShaders) {
	mat4 mv = projection * view * model;
	currentShaders.setUniform("ModelIn", model);
	currentShaders.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	currentShaders.setUniform("MVP", mv);

	currentShaders.setUniform("CamPos", cameraPosition);
}

// Game update function
void SceneBasic_Uniform::update( float t )
{
	float delta = t - lastFrame;
	lastFrame = t;

	vec3 tempPos = vec3(sinf(glfwGetTime()) * 60, 30.0f, cosf(glfwGetTime()) * 60); // Animating light position
	light1Position = tempPos;

}

// Pass 1 Render scene
void SceneBasic_Uniform::pass1() {
	model = mat4(1.0f);

	// ---- NEED TO RE-ADD TO STOP SKYBOX "MOVING" ----
	cameraPosition = playerObject.getCameraPosition(vec3(0.0f, -1.0f, 5.0f));
	model = mat4(1.0f);
	view = glm::lookAt(cameraPosition, cameraPosition + playerObject.getCamForward(), playerObject.getCamUp());


	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
	glEnable(GL_DEPTH_TEST);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rendering skybox
	skyboxShaders.use(); 
	positionModel(cameraPosition); 
	// Skybox rendering 
	setMatricies(skyboxShaders); 
	assignCubemap(skyboxTexture);
	
	
	sky.render(); 
	 
	standardShaders.use();
	standardShaders.setUniform("Pass", 1); // Define which render pass is being used

	positionModel(playerObject.getPosition());
	rotateModel(playerObject.getOrientation());
	setMatricies(standardShaders);
	assignMaterial(5.0f);
	// Standard obejct rendering
	assignTexture(plainTexture);

	
	playerObject.render(standardShaders);
	
	positionModel(vec3(0.0f));
	setMatricies(standardShaders);
	tempTesting->render();
	
}

// Pass 2 get bloom pixels
void SceneBasic_Uniform::pass2() {
	standardShaders.use(); 
	standardShaders.setUniform("Pass", 2);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO); 
	glActiveTexture(GL_TEXTURE5); 
	glBindTexture(GL_TEXTURE_2D, renderTex);
	 
	glDisable(GL_DEPTH_TEST); 
	glClear(GL_COLOR_BUFFER_BIT); 

	standardShaders.setUniform("ModelViewMatrix", mat4(1.0f)); 
	standardShaders.setUniform("NormalMatrix", mat3(1.0f)); 
	standardShaders.setUniform("MVP", mat4(1.0f)); 

	glBindVertexArray(fsQuad); 
	glDrawArrays(GL_TRIANGLES, 0, 6); 
}

// Pass 3 blur in one direction
void SceneBasic_Uniform::pass3() {
	standardShaders.use();
	standardShaders.setUniform("Pass", 3);

	glBindFramebuffer(GL_FRAMEBUFFER, verticalFBO); 
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, lightTex);

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	standardShaders.setUniform("ModelViewMatrix", mat4(1.0f));
	standardShaders.setUniform("NormalMatrix", mat3(1.0f));
	standardShaders.setUniform("MVP", mat4(1.0f));

	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Pass 3 blur in one direction
void SceneBasic_Uniform::pass4() {
	standardShaders.use();
	standardShaders.setUniform("Pass", 4);

	glBindFramebuffer(GL_FRAMEBUFFER, horizontalFBO);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, verticalTex);

	glClear(GL_COLOR_BUFFER_BIT);

	standardShaders.setUniform("ModelViewMatrix", mat4(1.0f));
	standardShaders.setUniform("NormalMatrix", mat3(1.0f));
	standardShaders.setUniform("MVP", mat4(1.0f));

	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Pass 5 mix bloom and main scene
void SceneBasic_Uniform::pass5() {
	standardShaders.use();
	standardShaders.setUniform("Pass", 5);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, horizontalTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, renderTex);

	glClear(GL_COLOR_BUFFER_BIT);

	standardShaders.setUniform("ModelViewMatrix", mat4(1.0f)); 
	standardShaders.setUniform("NormalMatrix", mat3(1.0f)); 
	standardShaders.setUniform("MVP", mat4(1.0f)); 

	glBindVertexArray(fsQuad); 
	glDrawArrays(GL_TRIANGLES, 0, 6);  
}

void SceneBasic_Uniform::render()
{
	pass1();
	
	pass2();

	pass3();
	pass4();
	
	pass5();

	//testTorus.render();
}

void SceneBasic_Uniform::manageInput(GLFWwindow* myWindow) {
	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS) {
		playerObject.moveLocal(1, vec3(0.0f, 0.0f, 1.0f)); }
	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS) {
		playerObject.moveLocal(-1, vec3(0.0f, 0.0f, 1.0f)); }

	if (glfwGetKey(myWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
		playerObject.rotateLocal(5, vec3(0.0f, 1.0f, 0.0f)); }
	if (glfwGetKey(myWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		playerObject.rotateLocal(-5, vec3(0.0f, 1.0f, 0.0f)); }

	if (glfwGetKey(myWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		playerObject.rotateLocal(5, vec3(1.0f, 0.0f, 0.0f)); }
	if (glfwGetKey(myWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		playerObject.rotateLocal(-5, vec3(1.0f, 0.0f, 0.0f)); }

	if (glfwGetKey(myWindow, GLFW_KEY_Q) == GLFW_PRESS) {
		playerObject.rotateLocal(5, vec3(0.0f, 0.0f, 1.0f)); }
	if (glfwGetKey(myWindow, GLFW_KEY_E) == GLFW_PRESS) {
		playerObject.rotateLocal(-5, vec3(0.0f, 0.0f, 1.0f));
	}

}

// Unused manage mouse inputs
void SceneBasic_Uniform::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	//if (!mouseEntered)
	//{
	//	lastMousePos.x = (float)xpos;
	//	lastMousePos.y = (float)ypos;
	//	mouseEntered = true;
	//}

	////Sets values for change in position since last frame to current frame
	//float xOffset = (float)xpos - lastMousePos.x;
	//float yOffset = lastMousePos.y - (float)ypos;

	////Sets last positions to current positions for next frame
	//lastMousePos.x = (float)xpos;
	//lastMousePos.y = (float)ypos;

	////Moderates the change in position based on sensitivity value
	//const float sensitivity = 0.1f;
	//xOffset *= sensitivity;
	//yOffset *= sensitivity;

	////Adjusts yaw & pitch values against changes in positions
	//yaw += xOffset;
	//pitch += yOffset;

	////Prevents turning up & down beyond 90 degrees to look backwards
	//if (pitch > 89.0f)
	//{
	//	pitch = 89.0f;
	//}
	//else if (pitch < -89.0f)
	//{
	//	pitch = -89.0f;
	//}

	////Modification of direction vector based on mouse turning
	//vec3 direction;
	//direction.x = cos(radians(yaw)) * cos(radians(pitch));
	//direction.y = sin(radians(pitch));
	//direction.z = sin(radians(yaw)) * cos(radians(pitch));
	//cameraForward = normalize(direction);

	//// Calculate the new up vector based on the current forward and world up vector
	//vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
	//vec3 right = normalize(cross(worldUp, cameraForward));
	//cameraUp = normalize(cross(cameraForward, right));
}

void SceneBasic_Uniform::positionModel(vec3 newPosition) {
	model = mat4(1.0f);
	model = translate(model, newPosition);
}

void SceneBasic_Uniform::rotateModel(float rotation, vec3 axis) { 
	model = rotate(model, rotation, axis);
}

void SceneBasic_Uniform::rotateModel(mat4 orientation) {
	model = model * orientation;
}

void SceneBasic_Uniform::setScale(float scale) {
	model = glm::scale(model, vec3(scale));
}

void SceneBasic_Uniform::assignMaterial(float shininess) {
	standardShaders.setUniform("Material.Shininess", shininess); // Shininess
}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	

	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 200.0f);
}


void SceneBasic_Uniform::assignTexture(GLuint albedo) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);
}

void SceneBasic_Uniform::assignCubemap(GLuint albedo) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, albedo);
}
