#pragma once

#include "Wall.hpp"
#include "Animated.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

namespace Game {

/**
 * A breakable wall
 */
class BreakableWall : 
	public Game::Wall,
	public Game::Lifed,
	public Game::Animated,
	public Game::Scored,
	public Game::Sounded	
{
	bool destroyed = false;

protected:
	/** Constructor used by TransparentWall */
	BreakableWall(const sf::Vector2f& pos, const std::string& texture_name, unsigned short life = 1);
public:
	BreakableWall(const sf::Vector2f& pos, const unsigned short id, unsigned short life = 1);

	// Use animatedSprite, not the default sprite.
	void draw(sf::RenderTarget& window) override;
	void setOrigin(const sf::Vector2f& _origin) override {
		Game::Animated::setOrigin(_origin);
	}

	void destroy();
	bool isDestroyed() const { return destroyed && !animatedSprite.isPlaying(); }
};

}
