#include "Player.h"
#include <iostream>




Player::Player() {
	playerObject = ObjMesh::load("media/Ship.obj", true);

}

void Player::update(float delta) {
	if (rotationInput.x == 0.0f && rotationInput.y == 0.0f && rotationInput.z == 0.0f) {
		angularVelocity *= exp(-rotationalDamping * delta);
	}
	if (movementInput == 0.0f) {
		linearVelocity *= exp(-linearDamping * delta);
	}

	linearVelocity = clamp(linearVelocity + (movementInput * (movementAccel * delta)), -maxLinSpeed, maxLinSpeed);
	angularVelocity = clamp(angularVelocity + (rotationInput * (rotationAccel * delta)), -maxRotSpeed, maxRotSpeed);

	moveLocal(linearVelocity * delta, vec3(0.0f, 0.0f, 1.0f));

	rotateLocal(angularVelocity.x * delta, vec3(1.0f, 0.0f, 0.0f));
	rotateLocal(angularVelocity.y * delta, vec3(0.0f, 1.0f, 0.0f));
	rotateLocal(angularVelocity.z * delta, vec3(0.0f, 0.0f, 1.0f));

}

void Player::render(GLSLProgram& shader) {
	
	playerObject->render();
}

void Player::assignInputs(vec3 rotation, int movement) {
	rotationInput = rotation;
	movementInput = movement;
}

void Player::moveLocal(float motion, vec3 direction) {
	vec4 localDirection = orientation * vec4(direction, 1.0f);
	position += vec3(localDirection) * motion;
}

void Player::rotateLocal(float angle, vec3 axis) {
	axis = normalize(axis);

	quat rotation = angleAxis(radians(angle), axis);

	orientation = orientation * mat4_cast(rotation);
}

vec3 Player::getForward() { 
	vec3 baseForward = vec3(0.0f, 0.0f, 1.0f);
	mat4 inverseOrientation = inverse(orientation);
	return vec3(inverseOrientation * vec4(baseForward, 0.0f));
}
vec3 Player::getUp() { 
	vec3 baseUp = vec3(0.0f, 1.0f, 0.0f);
	mat4 inverseOrientation = inverse(orientation);
	return vec3(inverseOrientation * vec4(baseUp, 0.0f));
}
vec3 Player::getRight() {
	vec3 baseRight = vec3(1.0f, 0.0f, 0.0f);
	mat4 inverseOrientation = inverse(orientation);
	return vec3(inverseOrientation * vec4(baseRight, 0.0f));
}

vec3 Player::getCamUp() {
	return vec3(orientation[1]);
}
vec3 Player::getCamForward() { return vec3(orientation[2]); }

vec3 Player::getPosition() { return position; }
mat4 Player::getOrientation() { return orientation; }
//vec3 Player::getRotation() { return rotation; }

vec3 Player::getCameraPosition(vec3 offset) {
	vec4 localOffset = orientation * -vec4(offset, 1.0f);
	return position + vec3(localOffset);
}


// *UNUSED* there is no longer requirements for euler angles within the player
//mat4 Player::eulerToMat4(vec3 euler) {
//	mat4 rotationMatrix = mat4(1.0f);
//	rotationMatrix = rotate(rotationMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
//	rotationMatrix = rotate(rotationMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
//	rotationMatrix = rotate(rotationMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
//
//	return rotationMatrix;
//}