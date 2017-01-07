#pragma once

#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace lif {

class Animated;

class HauntedStatue : public lif::Entity, public sf::Drawable {
	bool possessed = false;

	lif::Animated *animated = nullptr,
	               *spirit = nullptr;

public:
	explicit HauntedStatue(const sf::Vector2f& pos);

	void setPossessed(bool b) { possessed = b; }
	bool isPossessed() const { return possessed; }

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
};

}
