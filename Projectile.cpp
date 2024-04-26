#include "Projectile.h"

Projectile::Projectile(vec3 playerPos) {
	asteroid = ObjMesh::load("media/Asteroid.obj", true);

	std::seed_seq seed{ static_cast<unsigned int>(time(NULL)) };
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(1, 6);

	// Pick random size 1 - 6
	int randomI = distribution(generator);

	std::uniform_real_distribution<float> locationDist(-100, 100);

	// Pick position on that side
	vec2 pos = vec2(locationDist(generator), locationDist(generator));

	if (randomI == 1) {
		position = vec3(150, pos.x, pos.y);
	}
	if (randomI == 2) {
		position = vec3(-150, pos.x, pos.y);
	}
	if (randomI == 3) {
		position = vec3(pos.x, 150, pos.y);
	}
	if (randomI == 4) {
		position = vec3(pos.x, -150, pos.y);
	}
	if (randomI == 5) {
		position = vec3(pos.x, pos.y, 150);
	}
	if (randomI == 6) {
		position = vec3(pos.x, pos.y, -150);
	}

	movementDir = normalize(playerPos - position);
}

void Projectile::update(float delta) {
	position += movementDir * 0.2f;
}

void Projectile::render() {
	asteroid->render();
}