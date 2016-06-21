#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Killable.hpp"
#include "game_values.hpp"

namespace Game {

class Player;

/**
 * The players' bomb. Upon explosion, a Game::Explosion is spawned
 * where the bomb was deployed.
 */
class Bomb : public Game::Entity {
	Game::Clock *fuseClock = nullptr;
	sf::Time fuseTime;
	unsigned short radius;
	
	bool ignited = false;
	bool exploded = false;
	bool switched = false;

	Game::Animated *animated = nullptr;
	Game::Killable *killable = nullptr;

	/** The player who dropped this bomb */
	const Game::Player *const sourcePlayer;

public:

	explicit Bomb(const sf::Vector2f& pos, const Game::Player *const source, 
			const sf::Time& fuseTime = Game::Conf::Bomb::DEFAULT_FUSE, 
			const unsigned short radius = Game::Conf::Bomb::DEFAULT_RADIUS);

	void update() override;
	
	/** `true` if this bomb was driven to explode by another explosion */
	bool isIgnited() const { return ignited; }
	/** Manually set this bomb to explode after 50 ms */
	void ignite();

	unsigned short getRadius() const { return radius; }
	void setRadius(unsigned short r) { radius = r; }

	void setFuseTime(const sf::Time& ft) { fuseTime = ft; }

	const Game::Player* getSourcePlayer() const { return sourcePlayer; }
};

}
