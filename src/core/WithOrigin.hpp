#pragma once

#include <SFML/System.hpp>

namespace Game {

class WithOrigin {
protected:
	sf::Vector2f origin;
public:
	virtual ~WithOrigin() {}

	virtual void setOrigin(const sf::Vector2f& o) {
		origin = o;
	}
};

}
