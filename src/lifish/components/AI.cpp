#include "AI.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "LevelManager.hpp"
#include "Sounded.hpp"
#include "Enemy.hpp"
#include "GameCache.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include <random>
#include <exception>

#define NEW_DIRECTION(d) \
{ \
	moving->setDirection(d); \
	return; \
}
#define SAME_DIRECTION return;
#define HANDLE_NOT_MOVING \
	if (!moving->isMoving()) \
		SAME_DIRECTION
#define HANDLE_UNALIGNED \
	if (!entity.isAligned()) { \
		if (collider->collidesWithSolid()) \
			NEW_DIRECTION(adjust_prev_align(entity, *moving)) \
		else \
			SAME_DIRECTION \
	} 

using Game::AIBoundFunction;
using D = Game::Direction;

std::array<Game::AIFunction, Game::AI_FUNCTION_NUM> Game::ai_functions = {{
	ai_random,
	ai_random_forward,
	ai_random_forward_haunt,
	ai_follow,
	ai_follow_dash,
	ai_chase
}};

static Game::Direction directions[] = {
	D::UP, D::RIGHT, D::DOWN, D::LEFT
};

static Game::Direction select_random_viable(
		const Game::AxisMoving& moving,
		const Game::LevelManager& lm, 
		const Game::Direction opp)
{
	Game::Direction dirs[4];
	unsigned short n = 0;
	for (const auto& d : directions)
		if (moving.canGo(d, lm) && d != opp) dirs[n++] = d;
	if (n == 0)
		dirs[n++] = opp;
	std::uniform_int_distribution<int> dist(0, n - 1);
	return dirs[dist(Game::rng)];
}

static Game::Direction seeing_player(const Game::LevelManager& lm, const Game::Sighted& sighted) {
	const auto& seen = sighted.entitiesSeen();
	Game::Direction dir = Game::Direction::NONE;
	unsigned short dist = Game::LEVEL_WIDTH + 1;
	for (unsigned short i = 0; i < 4; ++i) {
		for (const auto& pair : seen[i]) {
			if (lm.isPlayer(pair.first) && pair.second < dist) {
				dir = static_cast<Game::Direction>(i);
				dist = pair.second;
			}
		}
	}
	return dir;
}

/** To be called when `entity` is colliding and is not aligned:
 *  sets `moving.prevAlign` to the tile it'd have reached if it hadn't collided,
 *  and returns oppositeDirection(moving.direction).
 */
static Game::Direction adjust_prev_align(Game::Entity& entity, Game::AxisMoving& moving) {
	switch (moving.getDirection()) {
	case D::LEFT: 
	case D::UP:
		moving.setPrevAlign(Game::tile(entity.getPosition()));
		break;
	case D::RIGHT:
		moving.setPrevAlign(Game::tile(entity.getPosition()) + sf::Vector2i(1, 0));
		break;
	case D::DOWN:
		moving.setPrevAlign(Game::tile(entity.getPosition()) + sf::Vector2i(0, 1));
		break;
	case D::NONE:
		break;
	}
	moving.setDashing(false);

	return Game::oppositeDirection(moving.getDirection());
}

AIBoundFunction Game::ai_random(Game::Entity& entity) {
	auto moving = entity.get<Game::AxisMoving>();
	const auto collider = entity.get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random has no Moving or Collider component!");
	moving->setAutoRealignEnabled(false);

	return [&entity, moving, collider] (const Game::LevelManager& lm) { 
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		if (!collider->isColliding()) {
			if (moving->getDistTravelled() < Game::TILE_SIZE) 
				SAME_DIRECTION

			std::uniform_int_distribution<int> dist(0, 10);
			if (moving->getDistTravelled() < 2 * Game::TILE_SIZE && dist(Game::rng) <= 4)
				SAME_DIRECTION
		}
		moving->setDistTravelled(0);
		D dirs[4];
		unsigned short n = 0;
		if (entity.isAligned('x')) {
			if (moving->canGo(D::UP, lm)) dirs[n++] = D::UP;
			if (moving->canGo(D::DOWN, lm)) dirs[n++] = D::DOWN;
		}
		if (entity.isAligned('y')) {
			if (moving->canGo(D::LEFT, lm)) dirs[n++] = D::LEFT;
			if (moving->canGo(D::RIGHT, lm)) dirs[n++] = D::RIGHT;
		}
		if (n < 1) NEW_DIRECTION(D::NONE)
		
		std::uniform_int_distribution<int> d(0, n - 1);
		NEW_DIRECTION(dirs[d(Game::rng)])
	};
}

AIBoundFunction Game::ai_random_forward(Game::Entity& entity) {
	auto moving = entity.get<Game::AxisMoving>();
	const auto collider = entity.get<Game::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving"
				" or Collider component!");
	moving->setAutoRealignEnabled(false);

	return [&entity, moving, collider] (const Game::LevelManager& lm) { 
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		const D cur = moving->getDirection();
		const bool colliding = collider->collidesWithSolid();

		const D opp = oppositeDirection(cur);
		// colliding with a moving entity
		if (colliding && moving->canGo(cur, lm)) {
			NEW_DIRECTION(opp)
		}

		NEW_DIRECTION(select_random_viable(*moving, lm, opp))
	};
}

AIBoundFunction Game::ai_random_forward_haunt(Game::Entity& entity) {
	auto moving = entity.get<Game::AxisMoving>();
	const auto collider = entity.get<Game::Collider>();
	auto shooting = entity.get<Game::Shooting>();
	if (moving == nullptr || collider == nullptr || shooting == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving, "
				"Shooting or Collider component!");
	moving->setAutoRealignEnabled(false);

	return [&entity, moving, collider, shooting] (const Game::LevelManager& lm) {
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		if (shooting->isShooting()) {
			const auto cur_align = Game::tile(entity.getPosition());
			const D cur = moving->getDirection();
			const bool colliding = collider->collidesWithSolid();
			if (colliding && moving->canGo(cur, lm)) 
				NEW_DIRECTION(oppositeDirection(cur))

			if (shooting->getAttackAlign() == cur_align)
				SAME_DIRECTION

			if (shooting->getAttackAlign().x < cur_align.x)
				NEW_DIRECTION(D::LEFT)
			else if (shooting->getAttackAlign().x > cur_align.x)
				NEW_DIRECTION(D::RIGHT)
			else if (shooting->getAttackAlign().y < cur_align.y)
				NEW_DIRECTION(D::UP)
			else
				NEW_DIRECTION(D::DOWN)

		} else if (shooting->getAttackAlign().x > 0) {
			D dir = D::DOWN;
			const auto cur_align = Game::tile(entity.getPosition());
			if (shooting->getAttackAlign().x < cur_align.x)
				dir =  D::LEFT;
			else if (shooting->getAttackAlign().x > cur_align.x)
				dir =  D::RIGHT;
			else if (shooting->getAttackAlign().y < cur_align.y)
				dir =  D::UP;
			shooting->setAttackAlign(sf::Vector2i(-1, shooting->getAttackAlign().y));
			NEW_DIRECTION(dir)
		}
		ai_random_forward(entity)(lm);
	};
}

AIBoundFunction Game::ai_follow(Game::Entity& entity) {
	auto moving = entity.get<Game::AxisMoving>();
	const auto collider = entity.get<Game::Collider>();
	const auto sighted = entity.get<Game::Sighted>();
	if (moving == nullptr || collider == nullptr || sighted == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has no Moving"
				", Collider or Sighted component!");
	moving->setAutoRealignEnabled(false);

	return [&entity, moving, collider, sighted] (const Game::LevelManager& lm) {
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		const D cur = moving->getDirection();
		const bool colliding = collider->collidesWithSolid();

		const D opp = oppositeDirection(cur);
		if (colliding && moving->canGo(cur, lm))
			NEW_DIRECTION(opp)

		auto sp = seeing_player(lm, *sighted);
		if (sp != Game::Direction::NONE) {	
			auto sounded = entity.get<Game::Sounded>();
			if (sounded != nullptr)
				Game::cache.playSound(sounded->getSoundFile(Game::Sounds::YELL));
			NEW_DIRECTION(sp)
		}

		NEW_DIRECTION(select_random_viable(*moving, lm, opp))
	};
}

AIBoundFunction Game::ai_follow_dash(Game::Entity& entity) {
	auto moving = entity.get<Game::AxisMoving>();
	const auto collider = entity.get<Game::Collider>();
	const auto sighted = entity.get<Game::Sighted>();
	const auto shooting = entity.get<Game::Shooting>();
	if (moving == nullptr || collider == nullptr || sighted == nullptr || shooting == nullptr)
		throw std::invalid_argument("Entity passed to ai_random_forward_haunt has "
				"no Moving, Collider, Sighted or Shooting component!");
	moving->setAutoRealignEnabled(false);

	return [&entity, shooting, moving, collider, sighted] (const Game::LevelManager& lm) {
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		const D cur = moving->getDirection();
		const D opp = oppositeDirection(cur);
		const bool colliding = collider->collidesWithSolid();

		// XXX: is this necessary?
		if (colliding) {
			moving->setDashing(false);
			if (moving->canGo(cur, lm))
				NEW_DIRECTION(opp)
		}

		if (moving->isDashing())
			SAME_DIRECTION

		auto sp = seeing_player(lm, *sighted);
		if (sp != Game::Direction::NONE) {
			if (!shooting->isRecharging()) {
				shooting->shoot();
				auto sounded = entity.get<Game::Sounded>();
				if (sounded != nullptr)
					Game::cache.playSound(sounded->getSoundFile(Game::Sounds::ATTACK));
			}
			NEW_DIRECTION(sp)
		}

		NEW_DIRECTION(select_random_viable(*moving, lm, opp))
	};
}

AIBoundFunction Game::ai_chase(Game::Entity& entity) {
	// TODO
	return ai_follow(entity);
}
