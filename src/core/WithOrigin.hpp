#pragma once

#include <SFML/System.hpp>

namespace lif {

class WithOrigin {
protected:
	sf::Vector2f origin;
public:
	virtual ~WithOrigin() {}

	sf::Vector2f getOrigin() const { return origin; }
	virtual void setOrigin(const sf::Vector2f& o) {
		origin = o;
	}
};

}
