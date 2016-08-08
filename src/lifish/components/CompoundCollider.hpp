#pragma once

#include <initializer_list>
#include "Collider.hpp"

namespace Game {

/** A CompoundCollider is a main Collider plus a collection of other Colliders.
 *  Neither the main collider nor the children colliders can have an `onCollision` callback, as this class
 *  is meant to be used only for its `isColliding`, `contains` and `getRect` methods.
 *  These methods are overridden to be the union of all the internal colliders.
 *  Properties like layer and phantomness are given by the main Collider.
 *  Since CollisionDetector doesn't know how to treat such a Collider properly, a CompoundCollider is always phantom.
 */
class CompoundCollider : public Game::Collider {
	std::vector<Game::Collider> colliders;

public:
	explicit CompoundCollider(Game::Entity& owner, 
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE),
			  const sf::Vector2f& offset = sf::Vector2f(0, 0),
			  std::initializer_list<Game::Collider> clds = {})
		: Game::Collider(owner, layer, size, offset, true)
		, colliders(clds)
	{}

	/** @return true if any collider is colliding */
	bool isColliding() const override;
	/** @return true if any collider contains `other` */
	bool contains(const Game::Collider& other) const override;
	/** @return The bounding rectangle of all colliders */
	sf::IntRect getRect() const override;

	const std::vector<Game::Collider>& getColliders() const { return colliders; }

	void update() override;
};

}
