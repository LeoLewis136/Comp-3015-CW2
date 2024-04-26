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

	void update(float delta);
	void render();

	float size = 1.0f;
	vec3 position = vec3(0.0f);
	vec3 movementDir = vec3(0.0f);
};