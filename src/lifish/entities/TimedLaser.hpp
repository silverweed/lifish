#pragma once

#include "Entity.hpp"
#include "collision_layers.hpp"
#include <initializer_list>
#include <vector>

namespace lif {

class Animated;

/**
 * A TimedLaser is an Entity which damages all the entities in a row/column of the
 * level after giving a visual warning. The damaged entities can be selected via
 * their collision layer.
 */
class TimedLaser : public lif::Entity {
public:
	enum class Orientation { HORIZONTAL, VERTICAL };

private:
	const unsigned short rowCol;
	const sf::Time warnDuration;
	const int damage;
	const std::vector<lif::c_layers::Layer> damagedLayers;

	std::vector<lif::Entity*> dmgEntities;
	bool isWarning = true;

	lif::Animated *animated = nullptr;

	sf::Time warnT = sf::Time::Zero;

	void _giveWarning();
public:
	/**
	 * @param rowCol The index of the row/column to affect (start from 0)
	 * @param ori Whether to affect a row (HORIZONTAL) or a column (VERTICAL)
	 * @param warnDuration How much time does the duration persist before the damage is dealt
	 * @param damage Amount of damage dealt
	 * @param damagedLayers (optional) the collision layers to involve in damage dealing. If empty, damage all.
	 */
	explicit TimedLaser(unsigned short rowCol,
			Orientation ori,
			const sf::Time& warnDuration,
			int damage,
			std::initializer_list<lif::c_layers::Layer> damagedLayers = {});

	void update() override;
};

}
