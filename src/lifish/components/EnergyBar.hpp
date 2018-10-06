#pragma once

#include "Component.hpp"
#include <SFML/Graphics.hpp>

namespace lif {

class Clock;
class Sprite;

/** Keeps track of the final boss's health */
class EnergyBar : public lif::Component, public sf::Drawable {

	static constexpr float SPEED = 100;

	lif::Sprite *bar = nullptr;
	lif::Sprite *emptyBar = nullptr;
	lif::Clock *moveClock = nullptr;
	bool moving = false;

public:
	explicit EnergyBar(lif::Entity& owner);

	void setEnergy(int amt);
	/** Makes the EnergyBar move upward till it leaves the screen */
	void moveOut();

	void update() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
