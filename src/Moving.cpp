#include "Moving.hpp"

using Game::Moving;

Moving::Moving(Game::Entity *const owner)
	: direction(Game::Direction::NONE)
	, Game::Component(owner)
{
	position = owner->get<Game::Position<float>>();	
}
