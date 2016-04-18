#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "ShadedText.hpp"

namespace Game {

class LevelNumText : public Game::Component, public sf::Drawable {
	Game::ShadedText *text;

public:
	explicit LevelNumText::LevelNumText(Game::Entity *const owner, 
			const unsigned short levelnum);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
