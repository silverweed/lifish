#pragma once

#include "Enemy.hpp"

namespace lif {

class Wisp : public lif::Enemy {

	static constexpr float IN_WALL_SPEED_REDUCTION = 0.3;

	bool inWall = false;

	bool _checkCollision(lif::Collider& c) override;
public:
	explicit Wisp(const sf::Vector2f& pos, const lif::EnemyInfo& info);

	void update() override;
};

}
