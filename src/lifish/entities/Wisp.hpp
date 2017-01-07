#pragma once

#include "Enemy.hpp"

namespace lif {

class Wisp : public lif::Enemy {

	bool inWall = false;

	bool _checkCollision(lif::Collider& c) override;
public:
	explicit Wisp(const sf::Vector2f& pos, const lif::EnemyInfo& info);

	void update() override;
};

}
