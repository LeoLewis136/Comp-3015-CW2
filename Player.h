#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_float.hpp>



#include "helper/objmesh.h"
#include "helper/glslprogram.h"

using namespace glm;

class Player {
public:
	float size = 0.2;

	Player();

	void update(float delta);
	void render(GLSLProgram& shader);

	void moveLocal(float mot, vec3 motion);
	void rotateLocal(float angle, vec3 axis);

	void assignInputs(vec3 rotation, int movement);

	vec3 getForward();
	vec3 getUp();
	vec3 getRight();

	vec3 getCamUp();
	vec3 getCamForward();

	vec3 getPosition();
	mat4 getOrientation();
	//vec3 getRotation();
	vec3 getCameraPosition(vec3 offset);

	mat4 orientation = mat4(1.0f);

private:
	

	// Variables to define movmenent stats
	float movementAccel = 10;
	float rotationAccel = 45;

	float rotationalDamping = 2.0; // The rotational drag effect
	float linearDamping = 0.8; // The linear drag effect

	float maxRotSpeed = 90; // The highest speed that the object can rotate
	float maxLinSpeed = 10; // The highest speed that the object can move forwards and backwards

	vec3 rotationInput = vec3(0.0f, 0.0f, 0.0f); // Stores what inputs were made for rotation in this frame
	int movementInput = 0; // Stores what input was made to move the object

	vec3 angularVelocity = vec3(0.0f, 0.0f, 0.0f);
	float linearVelocity = 0.0f; // Stores the current velocity forward to back


	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	//vec3 rotation = vec3(0.0f, 0.0f, 90.0f);
	 // The objects orientation (rotation) in the 3D space. Makes rotation calculations easier.

	//mat4 eulerToMat4(vec3 euler);

	std::unique_ptr<ObjMesh> playerObject;
};