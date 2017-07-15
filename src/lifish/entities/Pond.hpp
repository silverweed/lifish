#pragma once

#include <initializer_list>
#include <SFML/System.hpp>
#include "Entity.hpp"
#include "collision_layers.hpp"

namespace lif {

/**
 * An entity damaging whoever steps on it. Can either be used directly
 * (but in this case the sprite, damage and layer bitmask must be added manually)
 * or via a child class.
 */
class Pond : public lif::Entity {
protected:
	const sf::Vector2f size;

	/** Bitmask determining which layers get influenced by this Pond */
	unsigned damaged_layers = 0;
	int damage;

public:
	explicit Pond(const sf::Vector2f& pos, const sf::Vector2f& size,
			int damage, std::initializer_list<lif::c_layers::Layer> damaged);
};

}
