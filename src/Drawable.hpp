#pragma once

namespace Game {

class Drawable {
	virtual void draw(sf::RenderTarget& window) = 0;
};

}
