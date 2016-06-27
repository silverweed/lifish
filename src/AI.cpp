#include "AI.hpp"
#include "utils.hpp"
#include "Game.hpp"
#include "LevelManager.hpp"
#include "Sounded.hpp"
#include "Enemy.hpp"
#include "Shooting.hpp"
#include "GameCache.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include <random>
#include <exception>

using Game::AIBoundFunction;
using D = Game::Direction;

static Game::Direction directions[] = {
	D::UP, D::RIGHT, D::DOWN, D::LEFT
};

static Game::Direction selectRandomViable(
		const Game::AxisMoving *const moving,
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

	return [entity, moving, collider] (const Game::LevelManager *const lm) { 
		if (!collider->isColliding()) {
			if (moving->getDistTravelled() < Game::TILE_SIZE) 
				return moving->getDirection();

			std::uniform_int_distribution<int> dist(0, 10);
			if (moving->getDistTravelled() < 2 * Game::TILE_SIZE && dist(Game::rng) <= 4)
				return moving->getDirection();
		}
		moving->setDistTravelled(0);
		D dirs[4];
		unsigned short n = 0;
		if (entity->isAligned('x')) {
			if (moving->canGo(D::UP, lm)) dirs[n++] = D::UP;
			if (moving->canGo(D::DOWN, lm)) dirs[n++] = D::DOWN;
		}
		if (entity->isAligned('y')) {
			if (moving->canGo(D::LEFT, lm)) dirs[n++] = D::LEFT;
			if (moving->canGo(D::RIGHT, lm)) dirs[n++] = D::RIGHT;
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
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving"
				" or Collider component!");

	return [entity, moving, collider] (const Game::LevelManager *const lm) { 
		const D cur = moving->getDirection();
		const auto cur_align = Game::tile(moving->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		const D opp = oppositeDirection(cur);
		// colliding with a moving entity
		if (collider->isColliding() && moving->canGo(cur, lm))
			return opp;

		return selectRandomViable(moving, lm, opp);
	};
}

AIBoundFunction Game::ai_random_forward_haunt(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	auto shooting = entity->get<Game::Shooting>();
	if (moving == nullptr || collider == nullptr || shooting == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving, "
				"Shooting or Collider component!");

	return [entity, moving, collider, shooting] (const Game::LevelManager *const lm) {
		if (shooting->isShooting()) {
			const auto cur_align = Game::tile(entity->getPosition());
			const D cur = moving->getDirection();
			if (collider->isColliding() && moving->canGo(cur, lm)) 
				return oppositeDirection(cur);

			if (shooting->getAttackAlign() == cur_align)
				return cur;
			
			if (shooting->getAttackAlign().x < cur_align.x)
				return D::LEFT;
			else if (shooting->getAttackAlign().x > cur_align.x)
				return D::RIGHT;
			else if (shooting->getAttackAlign().y < cur_align.y)
				return D::UP;
			else
				return D::DOWN;
		} else if (shooting->getAttackAlign().x > 0) {
			D dir = D::DOWN;
			const auto cur_align = Game::tile(entity->getPosition());
			if (shooting->getAttackAlign().x < cur_align.x)
				dir =  D::LEFT;
			else if (shooting->getAttackAlign().x > cur_align.x)
				dir =  D::RIGHT;
			else if (shooting->getAttackAlign().y < cur_align.y)
				dir =  D::UP;
			shooting->setAttackAlign(sf::Vector2i(-1, shooting->getAttackAlign().y));
			return dir;
		}
		return ai_random_forward(entity)(lm);
	};
}

AIBoundFunction Game::ai_follow(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	auto enemy = dynamic_cast<Game::Enemy*>(entity);
	if (moving == nullptr || collider == nullptr || enemy == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving"
				" or Collider component, or is no Enemy!");

	return [enemy, moving, collider] (const Game::LevelManager *const lm) {
		const D cur = moving->getDirection();
		const auto cur_align = Game::tile(enemy->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		const D opp = oppositeDirection(cur);
		if (collider->isColliding() && moving->canGo(cur, lm))
			return opp;

		auto sp = enemy->seeingPlayer(*lm);
		if (sp != Game::Direction::NONE) {	
			Game::cache.playSound(enemy->get<Game::Sounded>()->getSoundFile(Game::Sounds::YELL));
			return sp;
		}

		return selectRandomViable(moving, lm, opp);
	};
}

AIBoundFunction Game::ai_follow_dash(Game::Entity *const entity) {
	auto moving = entity->get<Game::AxisMoving>();
	auto collider = entity->get<Game::Collider>();
	auto enemy = dynamic_cast<Game::Enemy*>(entity);
	if (moving == nullptr || collider == nullptr || enemy == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has "
				"no Moving or Collider component, or is no Enemy!!");

	return [enemy, moving, collider] (const Game::LevelManager *const lm) {
		const D cur = moving->getDirection();
		const D opp = oppositeDirection(cur);

		if (collider->isColliding()) {
			moving->setDashing(false);
			if (moving->canGo(cur, lm))
				return opp;
		}

		if (moving->isDashing())
			return cur;

		const auto cur_align = Game::tile(enemy->getPosition());
		if (moving->getPrevAlign() == cur_align && !collider->isColliding()) 
			return cur;

		auto sp = enemy->seeingPlayer(*lm);
		if (sp != Game::Direction::NONE) {
			// FIXME: check cooldown somewhere!
			if (moving->setDashing(true))
				Game::cache.playSound(enemy->get<Game::Sounded>()
						->getSoundFile(Game::Sounds::ATTACK));
			return sp;
		}

		return selectRandomViable(moving, lm, opp);
	};
}

AIBoundFunction Game::ai_chase(Game::Entity *const entity) {
	// TODO
	return ai_follow(entity);
}
