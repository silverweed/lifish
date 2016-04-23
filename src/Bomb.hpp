#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Clock.hpp"
#include "Animated.hpp"

namespace Game {

class Player;

/**
 * The players' bomb. Upon explosion, a Game::Explosion is spawned
 * where the bomb was deployed.
 */
class Bomb : public Game::Entity {
	Game::Clock<1> *fuseClock = nullptr;
	sf::Time fuseTime;
	unsigned short radius;
	
	bool ignited = false;
	bool exploded = false;
	bool switched = false;

	Game::Animated *animated = nullptr;

	/** The player who dropped this bomb */
	const Game::Player *const sourcePlayer;

public:
	const static sf::Time DEFAULT_FUSE; 
	constexpr static unsigned short DEFAULT_RADIUS = 2;
	constexpr static unsigned short MAX_RADIUS = 4;

	explicit Bomb(const sf::Vector2f& pos, const Game::Player *const source, 
			const sf::Time& fuseTime = DEFAULT_FUSE, 
			const unsigned short radius = DEFAULT_RADIUS);

	void update() override;

	/** `true` if this bomb was driven to explode by another explosion */
	bool isIgnited() const { return ignited; }

	/** `true` if this bomb is currently exploding */
	bool isExploding() const { 
		return fuseClock->getElapsedTime() >= fuseTime;
	}
	
	void setExploding();

	/** `true` if this bomb already spawned its explosion. */
	bool isExploded() const { return exploded; }
	
	/** Mark this bomb as exploded (cannot be ignited nor explode again */
	void blowUp() { exploded = true; }

	unsigned short getRadius() const { return radius; }
	void setRadius(unsigned short r) { radius = r; }

	void setFuseTime(const sf::Time& ft) { fuseTime = ft; }

	const Game::Player* getSourcePlayer() const { return sourcePlayer; }
};

}
