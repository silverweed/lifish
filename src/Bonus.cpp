#include "Bonus.hpp"

using Game::Bonus;
using Game::TILE_SIZE;

std::discrete_distribution<unsigned short> Game::bonusTypeDistribution
{ 
	1, // zapper
	1, // sudden death
	5, // max bombs
	4, // quick fuse
	5, // max range
	5, // health small
	4, // health full
	5, // shield
	5, // speedy
	665 // no bonus
};
	
Bonus::Bonus(const sf::Vector2f& pos, const unsigned short _type) :
	Game::FixedEntity(pos, Game::getAsset("test", "bonuses.png")),
	Game::Scored(VALUE),
	type(_type)
{
	sprite.setTextureRect(sf::IntRect(type * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}

void Bonus::draw(sf::RenderTarget& window) {
	const float s = expireClock.getElapsedTime().asSeconds();
	if (EXPIRE_TIME - s <= 3.) {
		const float diff = s - std::floor(s);
		if (5*diff - std::floor(5*diff) < 0.5)
			return;
	}
	Game::FixedEntity::draw(window);
}
