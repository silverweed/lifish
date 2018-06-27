#pragma once

#include "Component.hpp"
#include <unordered_map>

namespace lif {

class Killable;

/** A generic component that handles the typical death of a Killable entity.
 *  In particular, it handles the case where owner has an Animated, an AxisMoving,
 *  a MovingAnimator or a Sounded component.
 */
class RegularEntityDeath : public lif::Component {
	const sf::Time deathTime;
	lif::Killable *killable = nullptr;
	std::unordered_map<lif::Component*, bool> origAutoRealign;

public:
	explicit RegularEntityDeath(lif::Entity& owner, sf::Time deathTime);

	void kill();
	bool isKillInProgress() const;
	/** Restores all owner's components altered by kill() */
	void resurrect();

	lif::Entity* init() override;
};

}
