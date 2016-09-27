#include "Scored.hpp"
#include "game.hpp"

using Game::Scored;

void Scored::setTarget(unsigned short t) {
	if (target != -1) return; // can only set target once
	if (0 >= t || t > Game::MAX_PLAYERS)
		throw std::logic_error("setTarget set to a number greater than MAX_PLAYERS or < 1!");
	target = t;
}
