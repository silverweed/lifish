#pragma once

#include <SFML/System.hpp>
#include "Enemy.hpp"

namespace Game {

class LevelManager;
class Clock;

/**
 * An Enemy which tunnels through map periodically and spawns an Acid Pond when dying.
 */
class AlienPredator : public Game::Enemy {
	const static sf::Time POND_LIFETIME;
	const static sf::Time TUNNEL_PERIOD;
	const static sf::Time TUNNEL_TRANSITION_TIME;
	constexpr static unsigned short TUNNEL_N_FRAMES = 2;

	Game::Clock *tunnelClock = nullptr,     // keeps track of seconds since latest tunneling 
	            *tunnelAnimClock = nullptr; // used for tunneling animation
	bool tunneling = false,
	     tunnelChangedPosition = false;


	sf::Vector2f _findTunneledPosition(const Game::LevelManager& lm) const;

public:
	explicit AlienPredator(const sf::Vector2f& pos, const Game::EnemyInfo& info);

	void update() override;
};

}
