#include "Egg.hpp"

using Game::Egg;

Egg::Egg(const sf::Vector2f& pos) 
	: Game::MovingEntity(pos, Game::getAsset("test", "egg.png"))
{

}
