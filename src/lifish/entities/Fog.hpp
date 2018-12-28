#pragma once

#include "Entity.hpp"

namespace lif {

class FreeMoving;
class Sprite;

class Fog : public lif::Entity {
	/** The time to wait before changing the alpha */
	const sf::Time alphaDt;

	lif::FreeMoving *moving = nullptr;
	lif::Sprite *sprite = nullptr;

	sf::Time t = sf::Time::Zero;

	bool fading = false;

public:
	explicit Fog(float speed = 10, sf::Time alphaDt = sf::seconds(0.01));

	void update() override;
};

}
