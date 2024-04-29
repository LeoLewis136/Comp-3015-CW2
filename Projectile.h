#pragma once
#include <glm/glm.hpp>
#include <glm/ext/vector_float3.hpp>

#include "helper/objmesh.h"

#include <random>


using namespace glm;

// Projectile object to allow for player shooting
class Projectile {
private:
	std::unique_ptr<ObjMesh> asteroid;

public:
	Projectile(vec3 playerPos);

	bool update(float delta, vec3 playerPos);
	void render();

	float size = 1.0f;
	vec3 position = vec3(0.0f);
	mat4 orientation = mat4(1.0f); // Objects "Rotation"

	vec3 rotationAxis = vec3(0.0f);
	float rotationSpeed = 0.0f;

	vec3 movementDir = vec3(0.0f);
};