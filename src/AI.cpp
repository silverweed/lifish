#include "AI.hpp"
#include "Enemy.hpp"
#include "utils.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static std::default_random_engine rng;
static D directions[] = { D::UP, D::RIGHT, D::DOWN, D::LEFT };

// FIXME
AIBoundFunction Game::ai_random(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer*) { 
		auto cur_align = Game::tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) return enemy->getDirection();
		D dirs[4];
		unsigned short n = 0;
		if (enemy->isAligned('x')) {
			dirs[n++] = D::UP;
			dirs[n++] = D::DOWN;
		}
		if (enemy->isAligned('y')) {
			dirs[n++] = D::LEFT;
			dirs[n++] = D::RIGHT;
		}
		if (n < 1) return D::NONE;
		std::uniform_int_distribution<int> d(0, n - 1);
		return dirs[d(rng)];
	};
}

AIBoundFunction Game::ai_random_forward(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer *lr) { 
		D cur = enemy->getDirection();
		auto cur_align = Game::tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) return cur;
		D opp = oppositeDirection(cur);
		if (enemy->colliding && enemy->canGo(cur, lr))
			return opp;
		D dirs[4];
		unsigned short n = 0;
		for (const auto& d : directions)
			if (enemy->canGo(d, lr) && d != opp) dirs[n++] = d;
		if (n == 0)
			dirs[n++] = opp;
		std::uniform_int_distribution<int> dist(0, n - 1);
		return dirs[dist(rng)];
	};
}
