#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "Drawable.hpp"

namespace Game {

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity : public Game::Drawable {
protected:
	sf::Texture *texture;
	sf::Vector2f pos;
	
public:
	/** How this entity behaves with collisions */
	struct {
		bool players = true;
		bool enemies = true;
		bool bullets = true;
		bool explosions = true;
	} transparentTo;

	Entity(const sf::Vector2f& _pos, const std::string& texture_name = "");
	virtual ~Entity() {}

	const sf::Vector2f& getPosition() const { return pos; }
	virtual void setPosition(const sf::Vector2f& _pos) { pos = _pos; }

	bool isAligned(const char axis = 'b') const;

	virtual void setOrigin(const sf::Vector2f& origin) = 0;
};

}
