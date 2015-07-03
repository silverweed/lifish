#pragma once
/**
 * Base class for game entities (walls, enemies, players, ...)
 */

#include <string>
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"
#include "Game.hpp"

namespace Game {

class Entity : public Game::Drawable {
protected:
	sf::Texture texture;
	sf::Vector2f pos;
	sf::Sprite sprite;
public:
	/** How this entity behaves with collisions */
	struct {
		bool players = true;
		bool enemies = true;
		bool bullets = true;
	} transparentTo;


	Entity(sf::Vector2f _pos, const std::string& texture_name);
	virtual ~Entity() {}

	sf::Vector2f getPosition() const { return pos; }
	void setPosition(sf::Vector2f _pos) { pos = _pos; }

	virtual void draw(sf::RenderTarget& window) override;

	inline bool isAligned() const {
		return (unsigned short)pos.x % Game::TILE_SIZE == 0 && (unsigned short)pos.y % Game::TILE_SIZE == 0;
	}

	sf::FloatRect getBounds() const {
		return sf::FloatRect(pos.x, pos.y, Game::TILE_SIZE, Game::TILE_SIZE);
	}
};

}
