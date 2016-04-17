#pragma once

#include <SFML/Graphics.hpp>
#include "Texture.hpp"

namespace Game {

/**
 * A drawable non-animated sprite
 */
class Sprite : public Game::Component, public sf::Drawable {
protected:
	Game::Texture *texture;
	sf::Sprite sprite;

public:
	explicit Sprite(Game::Entity *const owner, const std::string& texture_name);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
