#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "FreeSighted.hpp"

using Game::ShootingPoint;

ShootingPoint::ShootingPoint(Game::Entity& owner, const Game::Attack& attack, float visionRadius)
	: Game::Component(owner)
{
	sighted = addComponent(new Game::FreeSighted(*this, visionRadius));
	shooting = addComponent(new Game::Shooting(*this, attack));
	autoShooting = addComponent(new Game::AutoShooting(*this));
}
