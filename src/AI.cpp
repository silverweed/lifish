#include "AI.hpp"
#include "Enemy.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static std::default_random_engine rng;

AIBoundFunction Game::ai_random(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer*) { 
		static unsigned short steps = 0;
		if (steps++ < 150 && !enemy->colliding) return enemy->getDirection();
		steps = 0;
		std::vector<D> dirs;
		if (enemy->isAligned('x')) {
			dirs.push_back(D::UP);
			dirs.push_back(D::DOWN);
		}
		if (enemy->isAligned('y')) {
			dirs.push_back(D::LEFT);
			dirs.push_back(D::RIGHT);
		}
		std::uniform_int_distribution<int> d(0, dirs.size() - 1);
		return dirs[d(rng)];
	};
}
