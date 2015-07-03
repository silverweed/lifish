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
	transparentTo.players = false;
}

void Player::stop() {
	// Discretize the movement: if the player was moving 
	// towards a certain direction, make him reach the next
	// tile before actually stopping.
	unsigned short ix = (unsigned short)pos.x,
		       iy = (unsigned short)pos.y;
	if (ix % TILE_SIZE != 0 || iy % TILE_SIZE != 0) {
		move(direction);
	} else {
		ix = (unsigned short)pos.x,
		iy = (unsigned short)pos.y;
		animatedSprite.setPosition(ix, iy);
		MovingEntity::stop();
	}
}
