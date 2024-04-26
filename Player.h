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
	Player();

	void update();
	void render(GLSLProgram& shader);

	void moveLocal(float mot, vec3 motion);
	void rotateLocal(float angle, vec3 axis);

	vec3 getForward();
	vec3 getUp();
	vec3 getRight();

	vec3 getCamUp();
	vec3 getCamForward();

	vec3 getPosition();
	mat4 getOrientation();
	//vec3 getRotation();
	vec3 getCameraPosition(vec3 offset);

private:
	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	//vec3 rotation = vec3(0.0f, 0.0f, 90.0f);
	mat4 orientation = mat4(1.0f); // The objects orientation (rotation) in the 3D space. Makes rotation calculations easier.

	//mat4 eulerToMat4(vec3 euler);

	std::unique_ptr<ObjMesh> playerObject;
};