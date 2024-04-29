#include "Bullet.h"

Bullet::Bullet(vec3 _position, vec3 _moveDir) {
	bullet = ObjMesh::load("media/Bullet.obj", true);

	position = _position;
	direction = _moveDir;
}

int Bullet::update(vec3 playerPos, float delta, std::vector<Projectile> &projectiles) {
	position += direction * movementSpeed * delta;

	for (int i = 0; i < projectiles.size(); i++) {
		if (distance(projectiles[i].position, position) < projectiles[i].size + size) {
			return i;
		}
	}

	if (position.x > playerPos.x + 130 || position.x < playerPos.x - 130) { return -1; } 
	if (position.y > playerPos.y + 130 || position.y < playerPos.y - 130) { return -1; } 
	if (position.z > playerPos.z + 130 || position.z < playerPos.z - 130) { return -1; } 

	return -2;
}

void Bullet::render() {
	bullet->render();
}