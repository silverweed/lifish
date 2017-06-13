#pragma once

#include "Explosion.hpp"

namespace lif {

class EnemyExplosion : public lif::Explosion {
public:
	/** Note: arguments are in different order than Explosion because here we
	 *  often don't want a default damage, so it has no default value.
	 */
	explicit EnemyExplosion(const sf::Vector2f& pos,
		unsigned short _radius,
		int damage,
		const lif::Entity *const source = nullptr,
		bool isIncendiary = false);
};

}
