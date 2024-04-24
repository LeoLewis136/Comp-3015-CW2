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

SceneBasic_Uniform::SceneBasic_Uniform() : sky(200) , water(60, 60, 1, 1) {}

float SceneBasic_Uniform::gauss(float x, float sigma2) {
	double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
	double exponent = -(-x * x) / (2.0 * sigma2);
	return (float)(coeff * exp(exponent));
}

void SceneBasic_Uniform::initScene()
{
	compile();

	treePositions.push_back(vec4(30.0f, 6.5f, 30.0f, 10.0f));
	treePositions.push_back(vec4(30.0f, 9.0f, -30.0f, 25.0f));
	treePositions.push_back(vec4(30.0f, 6.0f, 4.0f, 0.0f));
	treePositions.push_back(vec4(20.0f, 6.9f, -20.0f, 48.0f));
	treePositions.push_back(vec4(40.0f, 8.8f, -5.0f, 180.0f));
	treePositions.push_back(vec4(28.0f, 8.0f, -15.0f, 280.0f));
	treePositions.push_back(vec4(35.0f, 6.5f, 20.0f, 95.0f));
	treePositions.push_back(vec4(26.0f, 4.2f, 14.0f, 250.0f));
	treePositions.push_back(vec4(38.0f, 6.0f, 10.0f, 120.0f));
	treePositions.push_back(vec4(40.0f, 8.5f, -28.0f, 60.0f));

	model = mat4(1.0f);
	view = glm::lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	
	/* --- Unused --- multiple lights in scene
	To be reimplemented when normal mapping is complete */
	standardShaders.use();
	// ---- Point light setup ---- //
	

	// Scene light values
	// Specular
	standardShaders.setUniform("lights[0].Specular", vec3(1.0f, 0.98f, 0.86f)); // LD
	standardShaders.setUniform("lights[1].Specular", vec3(1.0f, 0.98f, 0.86f)); // LD

	standardShaders.setUniform("lights[0].Diffuse", vec3(0.4f, 0.4f, 0.4f)); // LD
	standardShaders.setUniform("lights[1].Diffuse", vec3(0.4f, 0.4f, 0.4f)); // LD

	// Ambient
	standardShaders.setUniform("lights[0].Ambient", vec3(0.1f, 0.1f, 0.1f)); // LA
	standardShaders.setUniform("lights[1].Ambient", vec3(0.1f, 0.1f, 0.1f)); // LA

	// Fog Setup
	standardShaders.setUniform("Fog.FogColour", vec3(0.2, 0.2, 0.2));
	standardShaders.setUniform("Fog.MinDist", 3.0f);
	standardShaders.setUniform("Fog.MaxDist", 30.0f);

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
	skyboxShaders.setUniform("Fog.FogColour", vec3(0.2, 0.2, 0.2));
	skyboxShaders.setUniform("Fog.MinDist", 3.0f);
	skyboxShaders.setUniform("Fog.MaxDist", 30.0f);



	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
	
	terrain = ObjMesh::load("media/terrain.obj", true);
	dock = ObjMesh::load("media/port.obj", true);
	lantern = ObjMesh::load("media/Lantern.obj", true);

	for (int i = 0; i < 10; i++) {
		trees.push_back(ObjMesh::load("media/tree.obj", true));
	}

	lanternTexture = Texture::loadTexture("media/texture/Lantern.png");
	woodTexture = Texture::loadTexture("media/texture/WoodTexture.jpg");
	woodNormal = Texture::loadTexture("media/texture/WoodNormal.jpg");
	skyboxTexture = Texture::loadCubeMap("media/texture/Skybox/vz_sinister", ".png");
	treeTexture = Texture::loadTexture("media/texture/Tree.png");
	terrainTexture = Texture::loadTexture("media/texture/Grass.png");
	flowerTexture = Texture::loadTexture("media/texture/Flowers.png");
	waterTexture = Texture::loadTexture("media/texture/Water.png");

	blankNormal = Texture::loadTexture("media/texture/TempNormal.jpg");
	blankMix = Texture::loadTexture("media/texture/TransparentTex.png");


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

	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	glGenTextures(1, &intermediateTex);	
	glBindTexture(GL_TEXTURE_2D, intermediateTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTex, 0);

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

// * Unused * 
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


	moveCamera(t);
}

void SceneBasic_Uniform::pass1() {
	model = mat4(1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
	glEnable(GL_DEPTH_TEST);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyboxShaders.use(); 
	positionModel(cameraPosition); 
	setMatricies(skyboxShaders); 
	// Skybox rendering 
	assignCubemap(skyboxTexture); 
	sky.render(); 
	 
	standardShaders.use();
	standardShaders.setUniform("Pass", 1); // Define which render pass is being used

	standardShaders.setUniform("lights[0].Position", light0Position); 
	standardShaders.setUniform("lights[1].Position", light1Position); 

	positionModel(vec3(0.0f));
	setMatricies(standardShaders);
	assignMaterial(5.0f);
	// Standard obejct rendering
	assignTexture(terrainTexture, blankNormal, flowerTexture);
	terrain->render();

	assignTexture(treeTexture , blankNormal, blankMix);
	for (int i = 0; i < trees.size(); i++) {
		positionModel(vec3(treePositions[i]));
		rotateModel(radians(treePositions[i].a), vec3(0, 1, 0));

		setMatricies(standardShaders);
		trees[i]->render();
	}

	positionModel(vec3(28.0f, 2.5f, 1.0f));
	setMatricies(standardShaders);
	assignTexture(lanternTexture, blankNormal, blankMix);
	lantern->render();

	positionModel(vec3(23.0f, 0.5f, 0.0f));
	setScale(2.0f);
	setMatricies(standardShaders);
	assignTexture(woodTexture, woodNormal, blankMix);
	dock->render();

	positionModel(vec3(0.0f));
	setMatricies(standardShaders);
	assignTexture(waterTexture, woodNormal, blankMix);
	assignMaterial(32.0f);
	water.render();

	
}

void SceneBasic_Uniform::pass2() {
	standardShaders.use();
	standardShaders.setUniform("Pass", 2);

	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO); 
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, renderTex);

	glDisable(GL_DEPTH_TEST); 
	glClear(GL_COLOR_BUFFER_BIT);

	standardShaders.setUniform("ModelViewMatrix", mat4(1.0f));
	standardShaders.setUniform("NormalMatrix", mat3(1.0f));
	standardShaders.setUniform("MVP", mat4(1.0f));

	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
}

void SceneBasic_Uniform::pass3() {
	standardShaders.use();
	standardShaders.setUniform("Pass", 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, intermediateTex);

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

	//testTorus.render();
}

void SceneBasic_Uniform::manageInput(GLFWwindow* myWindow) {
	movementVec = vec3(0.0f);

	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS) {
		movementVec.z = 1; }
	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS) {
		movementVec.z = -1; }

	if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS) {
		movementVec.x = 1; }
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS) {
		movementVec.x = -1; }
}

void SceneBasic_Uniform::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (!mouseEntered)
	{
		lastMousePos.x = (float)xpos;
		lastMousePos.y = (float)ypos;
		mouseEntered = true;
	}

	//Sets values for change in position since last frame to current frame
	float xOffset = (float)xpos - lastMousePos.x;
	float yOffset = lastMousePos.y - (float)ypos;

	//Sets last positions to current positions for next frame
	lastMousePos.x = (float)xpos;
	lastMousePos.y = (float)ypos;

	//Moderates the change in position based on sensitivity value
	const float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	//Adjusts yaw & pitch values against changes in positions
	yaw += xOffset;
	pitch += yOffset;

	//Prevents turning up & down beyond 90 degrees to look backwards
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	//Modification of direction vector based on mouse turning
	vec3 direction;
	direction.x = cos(radians(yaw)) * cos(radians(pitch));
	direction.y = sin(radians(pitch));
	direction.z = sin(radians(yaw)) * cos(radians(pitch));
	cameraForward = normalize(direction);

	// Calculate the new up vector based on the current forward and world up vector
	vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = normalize(cross(worldUp, cameraForward));
	cameraUp = normalize(cross(cameraForward, right));
}

void SceneBasic_Uniform::moveCamera(float t) {
	if (movementVec.z != 0) { cameraPosition += (cameraForward * movementVec.z) / t; }
	if (movementVec.x != 0) { cameraPosition -= normalize(cross((cameraForward * movementVec.x), cameraUp)) / t; }

	model = mat4(1.0f);
	view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
}

void SceneBasic_Uniform::positionModel(vec3 newPosition) {
	model = mat4(1.0f);
	model = translate(model, newPosition);
}

void SceneBasic_Uniform::rotateModel(float rotation, vec3 axis) { 
	model = rotate(model, rotation, axis);
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


void SceneBasic_Uniform::assignTexture(GLuint albedo, GLuint normal, GLuint albedo2) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, albedo2);
}

void SceneBasic_Uniform::assignCubemap(GLuint albedo) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, albedo);
}
