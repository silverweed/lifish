#include "AI.hpp"
#include "Enemy.hpp"
#include "utils.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static D directions[] = { D::UP, D::RIGHT, D::DOWN, D::LEFT };

static D selectRandomViable(const Game::Enemy *const enemy, const Game::LevelRenderer *const lr, const D opp) {
	D dirs[4];
	unsigned short n = 0;
	for (const auto& d : directions)
		if (enemy->canGo(d, lr) && d != opp) dirs[n++] = d;
	if (n == 0)
		dirs[n++] = opp;
	std::uniform_int_distribution<int> dist(0, n - 1);
	return dirs[dist(Game::rng)];
}


AIBoundFunction Game::ai_random(Game::Enemy *const enemy) {
	return [enemy] (const Game::LevelRenderer *const lr) { 
		if (!enemy->colliding) {
			if (enemy->distTravelled < Game::TILE_SIZE) 
				return enemy->getDirection();

			std::uniform_int_distribution<int> dist(0, 10);
			if (enemy->distTravelled < 2 * Game::TILE_SIZE && dist(Game::rng) <= 4)
				return enemy->getDirection();
		}
		enemy->distTravelled = 0;
		D dirs[4];
		unsigned short n = 0;
		if (enemy->isAligned('x')) {
			if (enemy->canGo(D::UP, lr)) dirs[n++] = D::UP;
			if (enemy->canGo(D::DOWN, lr)) dirs[n++] = D::DOWN;
		}
		if (enemy->isAligned('y')) {
			if (enemy->canGo(D::LEFT, lr)) dirs[n++] = D::LEFT;
			if (enemy->canGo(D::RIGHT, lr)) dirs[n++] = D::RIGHT;
		}
		if (n < 1) return D::NONE;
		std::uniform_int_distribution<int> d(0, n - 1);
		return dirs[d(Game::rng)];
	};
}

AIBoundFunction Game::ai_random_forward(Game::Enemy *const enemy) {
	return [enemy] (const Game::LevelRenderer *const lr) { 
		const D cur = enemy->getDirection();
		const auto cur_align = Game::tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) 
			return cur;

		const D opp = oppositeDirection(cur);
		// colliding with a moving entity
		if (enemy->colliding && enemy->canGo(cur, lr))
			return opp;

		return selectRandomViable(enemy, lr, opp);
	};
}

AIBoundFunction Game::ai_random_forward_haunt(Game::Enemy *const enemy) {
	return [enemy] (const Game::LevelRenderer *const lr) {
		if (enemy->isShooting()) {
			const auto cur_align = Game::tile(enemy->getPosition());
			const D cur = enemy->getDirection();
			if (enemy->colliding && enemy->canGo(cur, lr)) 
				return oppositeDirection(cur);

			if (enemy->attackAlign == cur_align)
				return cur;
			
			if (enemy->attackAlign.x < cur_align.x)
				return D::LEFT;
			else if (enemy->attackAlign.x > cur_align.x)
				return D::RIGHT;
			else if (enemy->attackAlign.y < cur_align.y)
				return D::UP;
			else
				return D::DOWN;
		} else if (enemy->attackAlign.x > 0) {
			D dir = D::DOWN;
			const auto cur_align = Game::tile(enemy->getPosition());
			if (enemy->attackAlign.x < cur_align.x)
				dir =  D::LEFT;
			else if (enemy->attackAlign.x > cur_align.x)
				dir =  D::RIGHT;
			else if (enemy->attackAlign.y < cur_align.y)
				dir =  D::UP;
			enemy->attackAlign.x = -1;
			return dir;
		}
		return ai_random_forward(enemy)(lr);
	};
}

AIBoundFunction Game::ai_follow(Game::Enemy *const enemy) {
	return [enemy] (const Game::LevelRenderer *const lr) {
		const D cur = enemy->getDirection();
		const auto cur_align = Game::tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) 
			return cur;

		const D opp = oppositeDirection(cur);
		if (enemy->colliding && enemy->canGo(cur, lr))
			return opp;

		if (enemy->seeingPlayer != Game::Direction::NONE)
			return enemy->seeingPlayer;

		return selectRandomViable(enemy, lr, opp);
	};
}

AIBoundFunction Game::ai_follow_dash(Game::Enemy *const enemy) {
	return [enemy] (const Game::LevelRenderer *const lr) {
		const D cur = enemy->getDirection();
		const D opp = oppositeDirection(cur);

		if (enemy->colliding) {
			enemy->setDashing(false);
			if (enemy->canGo(cur, lr))
				return opp;
		}

		if (enemy->isDashing())
			return cur;

		const auto cur_align = Game::tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) 
			return cur;

		if (enemy->seeingPlayer != Game::Direction::NONE) {
			enemy->setDashing(true);
			return enemy->seeingPlayer;
		}

		return selectRandomViable(enemy, lr, opp);
	};
}

AIBoundFunction Game::ai_chase(Game::Enemy *const enemy) {
	// TODO
	return ai_follow(enemy);
}
