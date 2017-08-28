#pragma once

#include "Boss.hpp"
#include "LevelManager.hpp"

namespace lif {

class Animated;
class Clock;
class FreeSighted;
class HurtDrawProxy;

class GodEyeBoss : public lif::Boss, public sf::Drawable {

	lif::LevelManager& lm;

	lif::Animated *spriteBg = nullptr,
	              *spriteFg = nullptr,
	              *shield = nullptr;
	lif::HurtDrawProxy *hurtDrawProxy = nullptr;
	lif::FreeSighted *sighted = nullptr;
	lif::Clock *shieldAlphaClock = nullptr,
	           *attackClock = nullptr;


	void _updatePupilPos();
	void _updateShield();
	void _teleportTo(const sf::Vector2f& newpos);
	void _shakeWalls();
public:
	explicit GodEyeBoss(const sf::Vector2f& pos, lif::LevelManager& lm);

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
