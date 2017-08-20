#pragma once

#include "Entity.hpp"

class Animation;

namespace lif {

class Collider;
class Animated;

/**
 * A breakable wall
 */
class BreakableWall : public lif::Entity {
	lif::Animated *animated = nullptr;

	lif::Entity* _spawnBonus();
protected:
	/** Constructor used by TransparentWall */
	BreakableWall(const sf::Vector2f& pos, const std::string& texture_name,
			unsigned life, unsigned int score);

	Animation& _setupAnimations(const std::string& texture_name);
	void _setupComponents(unsigned life, unsigned int score);

public:
	explicit BreakableWall(const sf::Vector2f& pos, const unsigned short id, unsigned life = 1);
};

}
