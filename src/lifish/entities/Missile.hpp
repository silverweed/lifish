#pragma once

#include "GuidedBullet.hpp"

namespace lif {

class Missile : public lif::GuidedBullet {
public:
	explicit Missile(const sf::Vector2f& pos, const sf::Vector2f& target,
		const lif::BulletInfo& info, const lif::Entity *const source = nullptr);

	void update() override;
};

}
