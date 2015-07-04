#include "AI.hpp"
#include "Enemy.hpp"
#include <random>

using Game::AIBoundFunction;
using D = Game::Direction;

static std::default_random_engine rng;

AIBoundFunction Game::ai_random(Game::Enemy *const enemy) {
	return [enemy] (const LevelRenderer *lr) { 
		static unsigned short steps = 0;
		if (steps++ < 200) return enemy->getDirection();
		steps = 0;
		std::uniform_int_distribution<int> d(0,3);
		switch(d(rng)) {
			case 0: return D::UP;
			case 1: return D::DOWN;
			case 2: return D::LEFT;
			case 3: return D::RIGHT;
		}
		return D::NONE; 
	};
}
