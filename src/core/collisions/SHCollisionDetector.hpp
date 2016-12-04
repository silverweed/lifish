#pragma once

#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "CollisionDetector.hpp"

namespace Game {

/** Allow the preferred number of the SpatialHashingCollisionDetector subdivisions
 *  to be tuned when compiling. For Lifish's purposes, 7 is a good choice.
 */
#ifndef DEFAULT_SHCD_SUBDIVISIONS
#	define DEFAULT_SHCD_SUBDIVISIONS 7
#endif
constexpr unsigned SHCD_SUBDIVISIONS = DEFAULT_SHCD_SUBDIVISIONS;
#undef DEFAULT_SHCD_SUBDIVISIONS

class Collider;

/**
 * Container for spatial hashing algorithm. Has `subdivision^2` buckets.
 */
class SHContainer {
	using Bucket = std::list<std::weak_ptr<Game::Collider>>;

	const sf::Vector2f levelSize,
	                   cellSize;
	const unsigned subdivisions;
	std::vector<Bucket> buckets;

	/** @return An unordered_set of bucket indexes for the buckets containing `obj`. */
	std::unordered_set<unsigned> _getIdFor(const Game::Collider& obj) const;

public:
	explicit SHContainer(const sf::Vector2f& levelSize, unsigned subdivisions);

	unsigned getSubdivisions() const { return subdivisions; }

	void clear();
	void insert(std::weak_ptr<Game::Collider> obj);
	/** @return A set of all colliders in an adjacent cell to `obj`. */
	auto getNearby(const Game::Collider& obj) const -> std::list<std::weak_ptr<Game::Collider>>;
};

/**
 * Implements a spatial hashing algorithm for collision detection.
 */
class SHCollisionDetector : public Game::CollisionDetector {
	SHContainer container;

public:
	explicit SHCollisionDetector(Game::EntityGroup& group, const sf::Vector2f& levelSize, unsigned subdivisions);

	void update() override;

	unsigned getSubdivisions() const { return container.getSubdivisions(); }
};

}
