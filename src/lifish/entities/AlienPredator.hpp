#pragma once

#include "Enemy.hpp"
#include <SFML/System.hpp>

namespace lif {

class LevelManager;

/**
 * An Enemy which tunnels through map periodically and spawns an Acid Pond when dying.
 */
class AlienPredator : public lif::Enemy {
	sf::Time tunnelT,
	         tunnelAnimT;
	bool tunneling = false,
	     tunnelChangedPosition = false;


	sf::Vector2f _findTunneledPosition(const lif::LevelManager& lm) const;

public:
	explicit AlienPredator(const sf::Vector2f& pos, const lif::EnemyInfo& info);

	void update() override;
};

}
