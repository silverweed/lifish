#pragma once

namespace Game {

class Drawable {
public:
	virtual void draw(sf::RenderTarget& window) = 0;
};

}
