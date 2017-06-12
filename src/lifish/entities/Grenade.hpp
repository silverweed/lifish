#pragma once

#include "AxisBullet.hpp"

namespace lif {

class Grenade : public lif::AxisBullet {
public:
	explicit Grenade(const sf::Vector2f& pos, lif::Direction dir,
			const lif::BulletInfo& info, const lif::Entity *const source = nullptr);

	void update() override;
};

}
