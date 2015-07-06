#include "AI.hpp"
#include "Enemy.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static std::default_random_engine rng;
static D directions[] = { D::UP, D::RIGHT, D::DOWN, D::LEFT };

AIBoundFunction Game::ai_random(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer*) { 
		if (enemy->steps++ < 150 && !enemy->colliding) return enemy->getDirection();
		enemy->steps = 0;
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
		std::uniform_int_distribution<int> d(0, n - 1);
		return dirs[d(rng)];
	};
}

AIBoundFunction Game::ai_random_forward(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer *lr) { 
		D cur = enemy->getDirection();
		auto cur_align = tile(enemy->getPosition());
		if (enemy->prevAlign == cur_align && !enemy->colliding) return cur;
		enemy->prevAlign = cur_align; 
		D opp = oppositeDirection(cur);
		if (enemy->colliding && enemy->canGo(cur, lr)) {
			// we're colliding with a moving entity
			return opp;
		}
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
