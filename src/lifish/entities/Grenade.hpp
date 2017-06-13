#pragma once

#include "GuidedBullet.hpp"
#include "Direction.hpp"

namespace lif {

class Grenade : public lif::GuidedBullet {
	sf::Vector2f _calculateEnd(const sf::Vector2f& pos, float range, lif::Direction dir,
			const lif::Entity *const target = nullptr) const;
public:
	explicit Grenade(const sf::Vector2f& pos, lif::Direction dir, const lif::BulletInfo& info,
			const lif::Entity *const source = nullptr,
			const lif::Entity *const target = nullptr);

	void update() override;
};

}
