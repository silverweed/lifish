#pragma once

#include "collision_layers.hpp"
#include <SFML/System.hpp>
#include <memory>
#include <vector>

namespace lif {

class Collider;
class Teleport;

class TeleportSystem final {
	// These 3 vectors all have the same length at all times
	std::vector<lif::Teleport*> teleports;
	/** Countdown to activation. If Zero, the teleport is active. */
	std::vector<sf::Time> inactiveTime;
	/** For each teleport, may contain the player that just teleported to it.
	 *  This is only done for players because they only get teleported again
	 *  if they're moving for long enough (this is to prevent them from getting
	 *  stuck in 1x1 teleporter tiles.)
	 */
	std::vector<std::weak_ptr<lif::Collider>> justTeleportedTo;

	void _updateActive(unsigned id);
	void _updateJustTeleportedTo(unsigned id);
	int _findNextViable(unsigned id, lif::c_layers::Layer layer) const;
public:
	explicit TeleportSystem() {}

	void add(lif::Teleport* tp);
	void clear();

	void update();
};

} // end  namespace lif
