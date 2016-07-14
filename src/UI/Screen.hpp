#pragma once

#include <SFML/Graphics.hpp>
#include "../Sprite.hpp"

namespace Game {

namespace UI {

class Screen : public sf::Drawable {
protected:
	/** The background tile */
	Game::Sprite *bgSprite = nullptr;

public:
	explicit Screen() {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}

}
