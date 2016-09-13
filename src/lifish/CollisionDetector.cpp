#include "CollisionDetector.hpp"
#include "game.hpp"
#include "CompoundCollider.hpp"
#include "Direction.hpp"
#include "AxisMoving.hpp"
#include "collision_layers.hpp"
#include <algorithm>
#include <iostream>
#include "Explosion.hpp"

using Game::Direction;
using Game::CollisionDetector;
using Game::TILE_SIZE;

// Checks if entity in `pos` and entity in `opos` collide, given that the one in `pos` has direction `dir`.
static bool collide(const Game::Collider& cld1, const Game::Collider& cld2, const Game::Direction dir) {
	if (auto cc1 = dynamic_cast<const Game::CompoundCollider*>(&cld1)) {
		for (const auto& c : cc1->getColliders())
			if (collide(cld2, c, dir)) {
				std::cerr << c.toString() << " colliding with " << cld2.toString() << std::endl;
				return true;
			}
		return false;
	} else if (auto cc2 = dynamic_cast<const Game::CompoundCollider*>(&cld2)) {
		for (const auto& c : cc2->getColliders())
			if (collide(c, cld1, dir)) {
				std::cerr << c.toString() << " colliding with " << cld1.toString() << std::endl;
				return true;
			}
		return false;
	}

	sf::IntRect rect = cld1.getRect(),
		    orect = cld2.getRect();

	switch (dir) {
	case Direction::UP: 
		--rect.top;
		break;
	case Direction::DOWN:
		++rect.top;
		break;
	case Direction::LEFT:  
		--rect.left;
		break;
	case Direction::RIGHT: 
		++rect.left;
		break;
	default:
		break;
	}

	return rect.intersects(orect);
}

static bool is_at_boundaries(const Game::Collider& cld, const Game::AxisMoving *const am) {
	const auto pos = cld.getOwner().getPosition();
	const auto rect = cld.getRect();
	if (am != nullptr) {
		auto dir = am->getDirection();
		switch (dir) {
		case Direction::UP:
			return pos.y <= TILE_SIZE;
		case Direction::LEFT:
			return pos.x <= TILE_SIZE;
		case Direction::DOWN:
			return pos.y + rect.height >= TILE_SIZE * (Game::LEVEL_HEIGHT + 1);
		case Direction::RIGHT:
			return pos.x + rect.width >= TILE_SIZE * (Game::LEVEL_WIDTH + 1);
		default:
			return false;
		}
	} else {
		return pos.y < TILE_SIZE || pos.x < TILE_SIZE 
			|| pos.x + rect.width > TILE_SIZE * Game::LEVEL_WIDTH
			|| pos.y + rect.height > TILE_SIZE * Game::LEVEL_HEIGHT;
	}
}

bool direction_is_viable(Game::Collider& cld, Game::AxisMoving& moving, Game::Collider& ocld) {
	const auto pos = cld.getOwner().getPosition();
	const auto opos = ocld.getOwner().getPosition();
	const auto size = cld.getRect();
	const auto osize = ocld.getRect();

	switch (moving.getDirection()) {
	case Direction::UP: return opos.y + osize.height <= pos.y + TILE_SIZE;
	case Direction::DOWN: return opos.y >= pos.y + size.height - TILE_SIZE;
	case Direction::LEFT: return opos.x + osize.width <= pos.x + TILE_SIZE;
	case Direction::RIGHT: return opos.x >= pos.x + size.width - TILE_SIZE;
	default: break;
	}

	return true;
}

void CollisionDetector::update() {
	/* For each moving entity, check (towards its direction):
	 * 1) has it reached the level boundaries?
	 * 2) is there another non-trasparent entity occupying the cell ahead?
	 */
	auto& colliding = group.collidingEntities;

	// Prune all expired colliders
	for (auto it = colliding.begin(); it != colliding.end(); ) {
		if (it->expired()) it = colliding.erase(it);
		else {
			auto collider = it->lock();
			// reset collider
			collider->colliding.clear();
			collider->atLimit = false;
			++it;
		}
	}
	
	// Collision detection loop
	for (auto it = colliding.begin(); it != colliding.end(); ++it) {
		auto collider = it->lock();

		if (collider->getOwner().get<Game::Fixed>() != nullptr)
			continue;

		auto moving = collider->getOwner().get<Game::AxisMoving>();
		if (is_at_boundaries(*collider, moving)) {
			collider->atLimit = true;	
			continue;
		}
		
		// Very simple (aka quadratic) check with all others
		for (auto jt = colliding.begin(); jt != colliding.end(); ++jt) {
			if (it == jt)  continue;

			auto othcollider = jt->lock();
			//if (dynamic_cast<const Game::Explosion*>(&collider->getOwner()))
				//std::cerr << collider->contains(*othcollider) << " " << collider->collidesWith(*othcollider) << std::endl;

			if (moving) {
				// Only check entities ahead of this one
				if (!direction_is_viable(*collider, *moving, *othcollider))
					continue;

				if (collider->collidesWith(*othcollider)
						&& collide(*collider, *othcollider, moving->getDirection()))
				{
					//std::cerr << &collider->getOwner() << " colliding with " << &othcollider->getOwner()<<std::endl;
					collider->colliding.push_back(*jt);
					if (othcollider->getOwner().get<Game::Moving>() == nullptr) {
						// Let the entity know we collided with it.
						// We only do that for non-moving entities to avoid problems with
						// multiple collisions between two moving entities.
						othcollider->colliding.push_back(*it);
						std::cerr << "[moving] colliding\n";
					}
				}
			} else if (collider->contains(*othcollider) && collider->collidesWith(*othcollider)) {
				std::cerr << &collider->getOwner() << " colliding with " << &othcollider->getOwner()<<std::endl;
				collider->colliding.push_back(*jt);
			}
		}
	}
}
