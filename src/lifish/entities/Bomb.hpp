#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"
#include "conf/bomb.hpp"

namespace Game {

class Player;
class Clock;
class Animated;
class Killable;

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
	/** Whether this bomb has already started the "near-explosion" animation or not */
	bool switched = false;
	/** An incendiary bomb will spawn Fire on explosion */ 
	bool incendiary = false;

	Game::Animated *animated = nullptr;
	Game::Killable *killable = nullptr;

	/** The player who dropped this bomb */
	const Game::Player& sourcePlayer;

public:

	explicit Bomb(const sf::Vector2f& pos, const Game::Player& source, 
			const sf::Time& fuseTime = Game::Conf::Bomb::DEFAULT_FUSE, 
			const unsigned short radius = Game::Conf::Bomb::DEFAULT_RADIUS,
			bool isIncendiary = false);

	void update() override;
	
	/** `true` if this bomb was driven to explode by another explosion */
	bool isIgnited() const { return ignited; }
	/** Manually set this bomb to explode after 50 ms */
	void ignite();

	unsigned short getRadius() const { return radius; }
	void setRadius(unsigned short r) { radius = r; }

	/** Returns true if this bomb's fuse is over and the bomb should blow off. */
	void setFuseTime(const sf::Time& ft) { fuseTime = ft; }

	void setIncendiary(bool b) { incendiary = b; }

	const Game::Player& getSourcePlayer() const { return sourcePlayer; }
};

}
