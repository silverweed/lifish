#pragma once

#include "Angle.hpp"
#include "Entity.hpp"

namespace lif {

class Sprite;

class SurgeWarn : public lif::Entity {

	const sf::Time duration;

	lif::Sprite *sprite = nullptr;

	sf::Time t = sf::Time::Zero;

public:
	explicit SurgeWarn(const sf::Vector2f& pos, const sf::Time& duration, const lif::Angle& rotation);

	void update() override;
};

}
