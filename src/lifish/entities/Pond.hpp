#pragma once

#include <initializer_list>
#include <SFML/System.hpp>
#include "Entity.hpp"
#include "collision_layers.hpp"

namespace Game {

/**
 * A Pond damaging whoever steps on it
 */
class Pond : public Game::Entity {
	/** Bitmask determining which layers get influenced by this Pond */
	unsigned damagedLayers = 0;
	int damage;

public:
	explicit Pond(const sf::Vector2f& pos, const std::string& spriteName, const sf::Vector2f& size, 
			int damage, std::initializer_list<Game::Layers::Layer> damaged);
};

}
