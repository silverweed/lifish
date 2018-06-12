#pragma once

#include "CollisionDetector.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

namespace lif {

constexpr static unsigned DEFAULT_SHCD_SUBDIVISIONS = 7;

class Collider;
class SHCollisionDetector;

/**
 * Container for spatial hashing algorithm. Has `subdivision^2` buckets.
 */
class SHContainer final {
	using Bucket = std::vector<std::weak_ptr<lif::Collider>>;

	friend class SHCollisionDetector;

	sf::Vector2f levelSize,
	             cellSize;
	unsigned subdivisions;
	std::vector<Bucket> buckets;
	Bucket all;

	/** @return A vector of bucket indexes for the buckets containing `obj`. */
	std::vector<unsigned> _getIdFor(const lif::Collider& obj) const;

public:
	SHContainer(const sf::Vector2f& levelSize, unsigned subdivisions);

	unsigned getSubdivisions() const { return subdivisions; }

	void clear();
	void insert(std::weak_ptr<lif::Collider> obj);
	/** @return A set of all colliders in an adjacent cell to `obj`. */
	auto getNearby(const lif::Collider& obj) const -> std::vector<std::weak_ptr<lif::Collider>>;
	/** @return The flattened vector of all colliders. This may differ from EntityGroup::getColliding
	 *  as the colliders which are actually considered by SHContainer are filtered through some
	 *  criteria (e.g. they must be active)
	 */
	auto getAll() const -> const std::vector<std::weak_ptr<lif::Collider>>& { return all; }
};

/**
 * Implements a spatial hashing algorithm for collision detection.
 */
class SHCollisionDetector final : public lif::CollisionDetector {
	SHContainer container;

public:
	explicit SHCollisionDetector(lif::EntityGroup& group,
				const sf::FloatRect& levelLimit = sf::FloatRect(0, 0, 0, 0),
				unsigned subdivisions = lif::DEFAULT_SHCD_SUBDIVISIONS);

	void update() override;

	unsigned getSubdivisions() const { return container.getSubdivisions(); }

	void setLevelLimit(const sf::FloatRect& limit) override;
};

}
