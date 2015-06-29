#pragma once
/**
 * Base class for game entities (walls, enemies, players, ...)
 */
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"

namespace Game {

class Entity : public Game::Drawable {
protected:
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Vector2f pos;
public:
	Entity(sf::Vector2f _pos);
	Entity(sf::Vector2f _pos, const std::string& texture_name);
	virtual ~Entity() {}

	sf::Sprite getSprite() const { return sprite; }

	sf::Vector2f getPos() const { return pos; }
	void setPos(sf::Vector2f _pos) { pos = _pos; }

	virtual void draw(sf::RenderTarget& win);
};

}
