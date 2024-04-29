#include "Projectile.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_float.hpp>

Projectile::Projectile(vec3 playerPos) {
	asteroid = ObjMesh::load("media/Asteroid.obj", true);

	std::seed_seq seed{ static_cast<unsigned int>(time(NULL)) };
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(1, 6);

	// Pick random size 1 - 6
	int randomI = distribution(generator);

	std::uniform_real_distribution<float> locationDist(-100, 100);
	std::uniform_real_distribution<float> rotationAmount(-90, 90);

	// Pick position on that side
	vec2 pos = vec2(locationDist(generator), locationDist(generator));

	if (randomI == 1) {
		position = vec3(120, pos.x, pos.y);
		
		// Calculate the target postition
		vec3 pos2 = vec3(0, locationDist(generator) / 2, locationDist(generator) / 2);
		movementDir = normalize((playerPos + pos2) - position);
	}
	if (randomI == 2) {
		position = vec3(-120, pos.x, pos.y);

		// Calculate the target postition
		vec3 pos2 = vec3(0, locationDist(generator) / 2, locationDist(generator) / 2);
		movementDir = normalize((playerPos + pos2) - position); 
	}
	if (randomI == 3) {
		position = vec3(pos.x, 120, pos.y);

		// Calculate the target postition
		vec3 pos2 = vec3(locationDist(generator) / 2, 0, locationDist(generator) / 2);
		movementDir = normalize((playerPos + pos2) - position); 
	}
	if (randomI == 4) {
		position = vec3(pos.x, -120, pos.y);

		// Calculate the target postition
		vec3 pos2 = vec3(locationDist(generator) / 2, 0, locationDist(generator) / 2); 
		movementDir = normalize((playerPos + pos2) - position); 
	}
	if (randomI == 5) {
		position = vec3(pos.x, pos.y, 120);

		// Calculate the target postition
		vec3 pos2 = vec3(locationDist(generator) / 2, locationDist(generator) / 2, 0);
		movementDir = normalize((playerPos + pos2) - position); 
	}
	if (randomI == 6) {
		position = vec3(pos.x, pos.y, -120);

		// Calculate the target postition
		vec3 pos2 = vec3(locationDist(generator) / 2, locationDist(generator) / 2, 0);
		movementDir = normalize((playerPos + pos2) - position); 
	}

	rotationAxis = normalize(vec3(rotationAmount(generator), rotationAmount(generator), rotationAmount(generator))); // Get a random rotation axis
	rotationSpeed = rotationAmount(generator); // Get the rotation speed per frame
}

bool Projectile::update(float delta, vec3 playerPos) {
	position += movementDir * 0.2f;

	// Get this object ready to delete
	if (position.x > playerPos.x + 130 || position.x < playerPos.x - 130) { return true; }
	if (position.y > playerPos.y + 130 || position.y < playerPos.y - 130) { return true; }
	if (position.z > playerPos.z + 130 || position.z < playerPos.z - 130) { return true; }

	quat rotation = angleAxis(radians(rotationSpeed) * delta, rotationAxis);
	orientation = orientation * mat4_cast(rotation);

	return false;
}

void Projectile::render() {
	asteroid->render();
}