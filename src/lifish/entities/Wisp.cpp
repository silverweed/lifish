#include "Wisp.hpp"
#include "Collider.hpp"
#include "collision_layers.hpp"
#include "AxisMoving.hpp"

using lif::Wisp;

Wisp::Wisp(const sf::Vector2f& pos, const lif::EnemyInfo& info) : lif::Enemy(pos, 2, info) {
	collider->setLayer(lif::c_layers::ENEMIES_IGNORE_BREAKABLES);	
}

void Wisp::update() {
	lif::Enemy::update();

	moving->setSpeed(1 - (inWall ? IN_WALL_SPEED_REDUCTION : 0), true);
	inWall = false;
}

bool Wisp::_checkCollision(lif::Collider& coll) {
	bool b = lif::Enemy::_checkCollision(coll);
	if (coll.getLayer() == lif::c_layers::BREAKABLES)
		inWall = true;
	return b;
}
