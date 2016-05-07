#include "AI.hpp"
#include "utils.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Moving.hpp"
#include "Collider.hpp"
#include "Direction.hpp"
#include <random>
#include <exception>

using Game::AIBoundFunction;
using D = Game::Direction;

static Game::Direction Game::selectRandomViable(
		const Game::Moving *const moving,
		const Game::LevelManager *const lm, 
		const Game::Direction opp)
{
	Game::Direction dirs[4];
	unsigned short n = 0;
	for (const auto& d : directions)
		if (moving->canGo(d, lm) && d != opp) dirs[n++] = d;
	if (n == 0)
		dirs[n++] = opp;
	std::uniform_int_distribution<int> dist(0, n - 1);
	return dirs[dist(Game::rng)];
}

AIBoundFunction Game::ai_random(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random has no Moving or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lr) { 
		if (!collider->isColliding()) {
			if (moving->distTravelled < Game::TILE_SIZE) 
				return moving->getDirection();

			std::uniform_int_distribution<int> dist(0, 10);
			if (moving->distTravelled < 2 * Game::TILE_SIZE && dist(Game::rng) <= 4)
				return moving->getDirection();
		}
		moving->distTravelled = 0;
		D dirs[4];
		unsigned short n = 0;
		if (entity->isAligned('x')) {
			if (moving->canGo(D::UP, lr)) dirs[n++] = D::UP;
			if (moving->canGo(D::DOWN, lr)) dirs[n++] = D::DOWN;
		}
		if (entity->isAligned('y')) {
			if (moving->canGo(D::LEFT, lr)) dirs[n++] = D::LEFT;
			if (moving->canGo(D::RIGHT, lr)) dirs[n++] = D::RIGHT;
		}
		if (n < 1) return D::NONE;
		std::uniform_int_distribution<int> d(0, n - 1);
		return dirs[d(Game::rng)];
	};
}

AIBoundFunction Game::ai_random_forward(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward has no Moving or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lr) { 
		const D cur = moving->getDirection();
		const auto cur_align = Game::tile(moving->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		const D opp = oppositeDirection(cur);
		// colliding with a moving entity
		if (collider->isColliding() && moving->canGo(cur, lr))
			return opp;

		return selectRandomViable(moving, lr, opp);
	};
}

AIBoundFunction Game::ai_random_forward_haunt(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lr) {
		if (entity->isShooting()) {
			const auto cur_align = Game::tile(entity->getPosition());
			const D cur = moving->getDirection();
			if (collider->isColliding() && moving->canGo(cur, lr)) 
				return oppositeDirection(cur);

			if (entity->attackAlign == cur_align)
				return cur;
			
			if (entity->attackAlign.x < cur_align.x)
				return D::LEFT;
			else if (entity->attackAlign.x > cur_align.x)
				return D::RIGHT;
			else if (entity->attackAlign.y < cur_align.y)
				return D::UP;
			else
				return D::DOWN;
		} else if (entity->attackAlign.x > 0) {
			D dir = D::DOWN;
			const auto cur_align = Game::tile(entity->getPosition());
			if (entity->attackAlign.x < cur_align.x)
				dir =  D::LEFT;
			else if (entity->attackAlign.x > cur_align.x)
				dir =  D::RIGHT;
			else if (entity->attackAlign.y < cur_align.y)
				dir =  D::UP;
			entity->attackAlign.x = -1;
			return dir;
		}
		return ai_random_forward(entity)(lr);
	};
}

AIBoundFunction Game::ai_follow(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lr) {
		const D cur = entity->getDirection();
		const auto cur_align = Game::tile(entity->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		const D opp = oppositeDirection(cur);
		if (collider->isColliding() && moving->canGo(cur, lr))
			return opp;

		if (entity->seeingPlayer != Game::Direction::NONE) {
			entity->yell();
			return entity->seeingPlayer;
		}

		return selectRandomViable(entity, lr, opp);
	};
}

AIBoundFunction Game::ai_follow_dash(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lr) {
		const D cur = moving->getDirection();
		const D opp = oppositeDirection(cur);

		if (collider->isColliding()) {
			entity->setDashing(false);
			if (moving->canGo(cur, lr))
				return opp;
		}

		if (entity->isDashing())
			return cur;

		const auto cur_align = Game::tile(entity->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		if (entity->seeingPlayer != Game::Direction::NONE) {
			if (entity->setDashing(true))
				Game::cache.playSound(entity->getSoundFile(Game::Sounds::ATTACK));
			return entity->seeingPlayer;
		}

		return selectRandomViable(entity, lr, opp);
	};
}

AIBoundFunction Game::ai_chase(Game::Entity *const entity) {
	// TODO
	return ai_follow(entity);
}
