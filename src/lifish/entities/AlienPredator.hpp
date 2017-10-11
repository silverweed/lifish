#pragma once

#include <SFML/System.hpp>
#include "Enemy.hpp"

namespace lif {

class LevelManager;
class Clock;

/**
 * An Enemy which tunnels through map periodically and spawns an Acid Pond when dying.
 */
class AlienPredator : public lif::Enemy {
	lif::Clock *tunnelClock = nullptr,     // keeps track of seconds since latest tunneling
	            *tunnelAnimClock = nullptr; // used for tunneling animation
	bool tunneling = false,
	     tunnelChangedPosition = false;


	sf::Vector2f _findTunneledPosition(const lif::LevelManager& lm) const;

public:
	explicit AlienPredator(const sf::Vector2f& pos, const lif::EnemyInfo& info);

	void update() override;
};

}
