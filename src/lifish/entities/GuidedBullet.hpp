#pragma once

#include "Bullet.hpp"
#include "GuidedMoving.hpp"

namespace lif {

/** A bullet with a GuidedMoving component. */
class GuidedBullet : public lif::Bullet {
public:
	explicit GuidedBullet(const sf::Vector2f& pos, const sf::Vector2f& end,
			const lif::BulletInfo& info, sf::Time timeTaken,
			const lif::Entity *const source = nullptr,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs = {});
};

}
