#pragma once

#include "Boss.hpp"

namespace lif {

class Animated;
class Clock;
class FreeSighted;
class HurtDrawProxy;

class GodEyeBoss : public lif::Boss, public sf::Drawable {

	lif::Animated *spriteBg = nullptr,
	              *spriteFg = nullptr,
	              *shield = nullptr;
	lif::HurtDrawProxy *hurtDrawProxy = nullptr;
	lif::FreeSighted *sighted = nullptr;
	lif::Clock *shieldAlphaClock = nullptr;


	void _updatePupilPos();
	void _updateShield();
	void _teleportTo(const sf::Vector2f& newpos);
public:
	explicit GodEyeBoss(const sf::Vector2f& pos);

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
