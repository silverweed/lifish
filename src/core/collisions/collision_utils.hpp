#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "Direction.hpp"

namespace lif {

class Collider;
class AxisMoving;
class BaseLevelManager;

namespace collision_utils {

/** Checks if `cld1` and `cld2` collide, given that the owner of `cld1` has direction `dir`. */
bool collide(const lif::Collider& cld1, const lif::Collider& cld2, const lif::Direction dir);

/** Checks if `cld` is at the level limit. */
bool is_at_boundaries(const lif::Collider& cld, const lif::AxisMoving *const am, const sf::FloatRect& limits);

/** Checks if `ocld` is along the forward direction of `cld` */
bool direction_is_viable(const lif::Collider& cld,
		const lif::AxisMoving& moving, const lif::Collider& ocld);

std::vector<sf::Vector2f> find_free_tiles(const lif::BaseLevelManager& lm);

}

}
