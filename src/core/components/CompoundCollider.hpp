#pragma once

#include <initializer_list>
#include "Collider.hpp"

namespace Game {

/** A CompoundCollider is a collection of Colliders.
 *  The `contains`, `isColliding` and `getRect` methods are overridden to be the union of all the internal colliders.
 *  @deprecated This class is not supported anymore in any way. Use multiple Colliders instead.
 */
class CompoundCollider : public Game::Collider {
	std::vector<Game::Collider> colliders;
	/** The rectangle containing all colliders */
	sf::IntRect boundingRect;


	void _calcBoundingRect();
public:
	explicit CompoundCollider(Game::Entity& owner, Game::Layers::Layer layer, 
			std::initializer_list<Game::Collider> clds);

	CompoundCollider(const Game::CompoundCollider& other);

	/** @return true if any collider is colliding */
	bool isColliding() const override;

	/** @return true if any collider contains `other` */
	bool contains(const Game::Collider& other) const override;

	/** @return true if `other` is contained in the bounding rect */
	bool containsLoose(const Game::Collider& other) const;

	/** @return all the subcolliders that collide with the given one */
	std::vector<Game::Collider> getSubCollidingWith(const Game::Collider& coll) const;

	/** @return The bounding rectangle of all colliders */
	sf::IntRect getRect() const override;

	/** @return The size of the bounding rectangle of all colliders */
	sf::Vector2i getSize() const override;

	const std::vector<Game::Collider>& getColliders() const { return colliders; }
};

}
