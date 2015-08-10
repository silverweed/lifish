#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"
#include "Game.hpp"

namespace Game {

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
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
		bool explosions = true;
	} transparentTo;


	Entity(const sf::Vector2f& _pos);
	Entity(const sf::Vector2f& _pos, const std::string& texture_name);
	virtual ~Entity() {}

	const sf::Vector2f& getPosition() const { return pos; }
	void setPosition(sf::Vector2f _pos) { pos = _pos; }

	virtual void draw(sf::RenderTarget& window) override;

	bool isAligned(const char axis = 'b') const;

	virtual const sf::Vector2f& getOrigin() const { return sprite.getOrigin(); }
	virtual void setOrigin(const sf::Vector2f& origin);

	virtual sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
};

}
