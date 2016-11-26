#include "Wisp.hpp"
#include "Collider.hpp"
#include "collision_layers.hpp"
#include "AxisMoving.hpp"

using Game::Wisp;

Wisp::Wisp(const sf::Vector2f& pos, const Game::EnemyInfo& info) : Game::Enemy(pos, 2, info) {
	collider->setLayer(Game::Layers::ENEMIES_IGNORE_BREAKABLES);	
}

void Wisp::update() {
	Game::Enemy::update();

	moving->setSpeed(1 - (inWall ? IN_WALL_SPEED_REDUCTION : 0), true);
	inWall = false;
}

void Wisp::_checkCollision(Game::Collider& coll) {
	Game::Enemy::_checkCollision(coll);
	if (coll.getLayer() == Game::Layers::BREAKABLES)
		inWall = true;
}
