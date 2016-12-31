#pragma once

#include "Enemy.hpp"

namespace Game {

class Wisp : public Game::Enemy {

	static constexpr float IN_WALL_SPEED_REDUCTION = 0.3;

	bool inWall = false;

	bool _checkCollision(Game::Collider& c) override;
public:
	explicit Wisp(const sf::Vector2f& pos, const Game::EnemyInfo& info);

	void update() override;
};

}
