#pragma once

#include "Animated.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

namespace Game {

/**
 * A breakable wall
 */
class BreakableWall
	: public Game::Animated
	, public Game::Lifed
	, public Game::Scored
	, public Game::Sounded
{
	bool destroyed = false;

protected:
	/** Constructor used by TransparentWall */
	BreakableWall(const sf::Vector2f& pos, const std::string& texture_name, unsigned short life = 1);
public:
	BreakableWall(const sf::Vector2f& pos, const unsigned short id, unsigned short life = 1);

	// Use animatedSprite, not the default sprite.
	void draw(sf::RenderTarget& window) override;

	void destroy();
	bool isDestroyed() const { return destroyed && !animatedSprite.isPlaying(); }
};

}
