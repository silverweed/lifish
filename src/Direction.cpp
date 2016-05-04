#include "Direction.hpp"

using Game::Direction;

static Direction directions[] = { 
	Direction::UP,
	Direction::RIGHT,
	Direction::DOWN, 
	Direction::LEFT 
};

Direction Game::turnRight(const Direction dir, short times) {
	if (dir == Direction::NONE) return Direction::NONE;

	int i = 0;
	for ( ; i < 4; ++i)
		if (directions[i] == dir) break;
	
	return directions[(4 + (i + times) % 4) % 4];
}
