#include "AI.hpp"
#include "Enemy.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static std::default_random_engine rng;
static D directions[] = { D::UP, D::RIGHT, D::DOWN, D::LEFT };

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

// FIXME
AIBoundFunction Game::ai_random_forward(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer *lr) { 
		/*static unsigned short steps = 0;
		if (enemy->colliding) {
			std::uniform_int_distribution<int> d(0, 3);
			return directions[d(rng)];			
		}*/
		std::vector<D> dirs;
		D cur = enemy->getDirection();
		if (enemy->isAligned('x')) {
			if ((enemy->colliding || cur != D::DOWN) && enemy->canGo(D::UP, lr)) dirs.push_back(D::UP);
			if ((enemy->colliding || cur != D::UP) && enemy->canGo(D::DOWN, lr)) dirs.push_back(D::DOWN);
		}
		if (enemy->isAligned('y')) {
			if ((enemy->colliding || cur != D::RIGHT) && enemy->canGo(D::LEFT, lr)) dirs.push_back(D::LEFT);
			if ((enemy->colliding || cur != D::LEFT) && enemy->canGo(D::RIGHT, lr)) dirs.push_back(D::RIGHT);
		}
		std::uniform_int_distribution<int> d(0, dirs.size() - 1);
		return dirs[d(rng)];
	};
}
