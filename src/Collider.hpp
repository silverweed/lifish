#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Game.hpp"
#include "collision_layers.hpp"

namespace Game {

class CollisionDetector;

class Collider : public Game::Component {
	friend class Game::CollisionDetector;

	using CollisionFunc = std::function<void(Game::Collider&)>;

	/** All the Colliders which are colliding with this one */
	std::vector<std::reference_wrapper<Game::Collider>> colliding;
	/** Whether this entity is at a level's boundary */
	bool atLimit = false;
	const sf::Vector2i size;
	/** If a Collider is phantom, it won't be automatically managed by EntityGroup
	 *  (and therefore CollisionDetector). A phantom collider may be useful if you
	 *  want to handle the collision logic yourself and you're only interested
	 *  in giving a collision layer to the collider.
	 */
	const bool phantom;
	/** Collision layer */
	Game::Layers::Layer layer;
	/** Optional callback to be called at every update */
	CollisionFunc onCollision;

public:
	explicit Collider(Game::Entity *const owner, 
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE),
			  bool phantom = false);

	explicit Collider(Game::Entity *const owner,
			  CollisionFunc onCollision,
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE), 
			  bool phantom = false);

	/** @return the list of Colliders colliding with this one */
	std::vector<std::reference_wrapper<Game::Collider>> getColliding() const { return colliding; }
	/** Manually sets `coll` to be colliding with this collider */
	void addColliding(Game::Collider& coll) { colliding.push_back(coll); }
	bool isColliding() const { return colliding.size() > 0; }

	/** @return the collision layer of this Collider */
	Game::Layers::Layer getLayer() const { return layer; }
	/** Sets the collision layer of this Collider */
	void setLayer(Game::Layers::Layer l) { layer = l; }

	bool isPhantom() const { return phantom; }

	bool contains(const Game::Collider& other) const {
		return getRect().intersects(other.getRect());
	}

	bool collidesWith(const Game::Collider& other) const {
		return Game::Layers::collide[layer][other.layer];
	}

	bool isSolidFor(const Game::Collider& other) const {
		return Game::Layers::solid[layer][other.layer];
	}

	/** @return whether this Collider is at the level boundary. This is set
	 *  externally by CollisionDetector.
	 */
	bool isAtLimit() const { return atLimit; }

	/** @return the bounding box of this Collider */
	sf::IntRect getRect() const {
		const auto pos = owner->getPosition();
		return sf::IntRect(pos.x, pos.y, size.x, size.y);
	}

	void update() override;
};

}
