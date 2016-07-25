#include "FreeBullet.hpp"
#include "FreeMoving.hpp"
#include <cmath>

using Game::FreeBullet;

FreeBullet::FreeBullet(const Game::Entity *const source, const double angle, const Game::Attack& attack)
	: Game::Bullet(source, attack)
{
	addComponent(new Game::FreeMoving(*this, attack.speed, sf::Vector2f(std::cos(angle), std::sin(angle))));

	// TODO texture, animations, etc
}
