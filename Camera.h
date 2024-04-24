#pragma once
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

class Camera {
private:
	vec3 position;

	vec3 position = vec3(0.0f, 0.0f, 0.0f); // The "Position" of the camera
	vec3 forward = vec3(0.0f, 0.0f, -1.0f); // Where the camera is facing
	vec3 up = vec3(0.0f, 1.0f, 0.0f); // The up direction of the camera

	float yaw = -90.0f;
	float pitch = 0.0f;

	float lastXPos;
	float lastYPos;
	bool mouseEntered = false;
	
	mat4 projection;
public:
	Camera();

};