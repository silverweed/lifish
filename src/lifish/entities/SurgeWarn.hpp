#pragma once

#include "Entity.hpp"
#include "Angle.hpp"

namespace lif {

class Clock;
class Sprite;

class SurgeWarn : public lif::Entity {
	
	const sf::Time duration;

	lif::Clock *clock = nullptr;
	lif::Sprite *sprite = nullptr;

public:
	explicit SurgeWarn(const sf::Vector2f& pos, const sf::Time& duration, const lif::Angle& rotation);

	void update() override;
};

}
