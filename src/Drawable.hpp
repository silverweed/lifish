#pragma once

#include <SFML/Graphics.hpp>

namespace Game {

class Drawable {
public:
	virtual void draw(sf::RenderTarget& window) = 0;
};

}
