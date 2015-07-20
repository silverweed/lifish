#include "Boss.hpp"
#include "Game.hpp"

using Game::Boss;

Boss::Boss(const sf::Vector2f& pos) :
	Game::FixedEntity(pos, Game::getAsset("test", "boss.png")),
	Game::Lifed(MAX_LIFE),
	Game::Scored(VALUE)
{
	sprite.setTextureRect(sf::IntRect(0, 0, SIZE * Game::TILE_SIZE, SIZE * Game::TILE_SIZE));

	shootingPoints[0] = pos + sf::Vector2f(34, 36);
	shootingPoints[1] = pos + sf::Vector2f(63, 36);
}

std::array<double, Boss::N_SHOOTING_POINTS>&& Boss::getShootingAngles(const sf::Vector2f& pos) const {
	std::array<double, N_SHOOTING_POINTS> angles;

	for (unsigned short i = 0; i < shootingPoints.size(); ++i) {
		// calculate angle with pos: a = pi - arctan(dy / dx)
		double dx = shootingPoints[i].x - pos.x,
		       dy = pos.y - shootingPoints[i].y;
		angles[i] = M_PI - std::atan2(dy, dx);
	}

	return std::move(angles);
}
