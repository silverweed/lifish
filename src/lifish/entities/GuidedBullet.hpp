#pragma once

#include <initializer_list>
#include <functional>
#include <vector>
#include "FreeBullet.hpp"
#include "GuidedMoving.hpp"

namespace lif {

/**
 * A GuidedBullet has a GuidedMoving moving behaviour.
 */
class GuidedBullet : public lif::FreeBullet {
public:
	explicit GuidedBullet(const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			const lif::BulletInfo& info,
			const lif::Entity *const source = nullptr,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs = {});
};

}
