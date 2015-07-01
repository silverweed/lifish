#include "Player.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <sstream>

using Game::Player;

Player::Player(sf::Vector2f pos, const unsigned short id) :
	// TODO: player sprites
	//MovingEntity(pos, Game::getAsset("graphics", std::string("player") + std::to_string(id) + std::string(".png"))), 
	MovingEntity(pos, Game::getAsset("test", std::string("player") + std::to_string(id) + std::string(".png"))), 
	Lifed(MAX_LIFE)
{
	speed = 150.f;
}
