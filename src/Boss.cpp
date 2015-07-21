#include "Boss.hpp"
#include "Game.hpp"
#include "utils.hpp"

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

void Boss::draw(sf::RenderTarget& window) {
	if (dead) {
		const float s = hurtClock.getElapsedTime().asSeconds();
		const float diff = s - std::floor(s);
		if (4*diff - std::floor(4*diff) < 0.5)
			return;
	} else if (isHurt) {
		if (hurtClock.getElapsedTime().asMilliseconds() > 200) {
			sprite.setColor(sf::Color(sf::Color::White));
			isHurt = false;
		} else {
			sprite.setColor(sf::Color(200, 0, 0, 255));
		}
	}
	Game::Entity::draw(window);
}

bool Boss::occupies(const sf::Vector2i& tile) const {
	const auto m_tile = Game::tile(pos);
	return tile.x >= m_tile.x && tile.x < m_tile.x + SIZE
		&& tile.y >= m_tile.y && tile.y < m_tile.y + SIZE;
}

void Boss::hurt() { 
	isHurt = true;
	hurtClock.restart(); 
}

void Boss::kill() {
	dead = true;
	sprite.setColor(sf::Color(sf::Color::White));
	hurtClock.restart();
}
