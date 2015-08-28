#pragma once

#include <SFML/Graphics.hpp>

namespace Game {

/**
 * A less restricting 'drawable' class than sf::Drawable which allows 
 * non-const draw() method.
 */
class Drawable {
public:
	virtual void draw(sf::RenderTarget& window) = 0;
};

}
