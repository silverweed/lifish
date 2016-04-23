#include "Moving.hpp"

using Game::Moving;

Moving::Moving(Game::Entity *const owner, float speed)
	: speed(speed)
	, direction(Game::Direction::NONE)
	, Game::Component(owner)
{}
