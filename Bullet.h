#pragma once
#include <glm/glm.hpp>
#include <glm/ext/vector_float3.hpp>

#include "helper/objmesh.h"

#include "Projectile.h"

using namespace glm;

class Bullet {
private:
	std::unique_ptr<ObjMesh> bullet;
public:
	Bullet(vec3 position, vec3 moveDir);

	int update(vec3 playerPos, float delta, std::vector<Projectile> &projectiles);
	void render();

	vec3 position = vec3(0.0f); // The position of the bullet
	vec3 direction = vec3(0.0f); // The direction the bullet moves

	float movementSpeed = 20; // The movement speed of the object
	float size = 0.1; // The size of this objects collision
};