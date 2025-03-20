#pragma once

#include "Entity.hpp"
#include "conf/bomb.hpp"
#include <SFML/System.hpp>

namespace lif {

class Player;
class Animated;
class Killable;

/**
 * The players' bomb. Upon explosion, a lif::Explosion is spawned
 * where the bomb was deployed.
 */
class Bomb : public lif::Entity {
	sf::Time fuseTime;
	sf::Time fuseT;
	unsigned short radius;

	bool ignited = false;
	bool exploded = false;
	/** Whether this bomb has already started the "near-explosion" animation or not */
	bool switched = false;
	/** An incendiary bomb will spawn Fire on explosion */
	bool incendiary = false;

	lif::Animated *animated = nullptr;
	lif::Killable *killable = nullptr;

	/** The entity who dropped this bomb */
	const lif::Entity *const sourceEntity = nullptr;

public:

	explicit Bomb(const sf::Vector2f& pos,
			const lif::Entity *const source = nullptr,
			const sf::Time& fuseTime = lif::conf::bomb::DEFAULT_FUSE,
			const unsigned short radius = lif::conf::bomb::DEFAULT_RADIUS);

	void update() override;

	/** `true` if this bomb was driven to explode by another explosion */
	bool isIgnited() const { return ignited; }
	/** Manually set this bomb to explode after 50 ms */
	void ignite();

	unsigned short getRadius() const { return radius; }
	void setRadius(unsigned short r) { radius = r; }

	/** Returns true if this bomb's fuse is over and the bomb should blow off. */
	void setFuseTime(const sf::Time& ft) { fuseTime = ft; }
	sf::Time getFuseTime() const { return fuseTime; }
	sf::Time getCurrentFuse() const;

	void setIncendiary(bool b) { incendiary = b; }

	const lif::Entity* getSourceEntity() const { return sourceEntity; }
};

}
