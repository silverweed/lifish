#include <exception>
#include "AutoShooting.hpp"

using Game::AutoShooting;

AutoShooting::AutoShooting(Game::Entity& owner)
	: Game::Component(owner)
{}

Game::Entity* AutoShooting::init() {
	shooting = owner.get<Shooting>();
	if (shooting == nullptr)
		throw std::logic_error("owner of AutoShooting has no Shooting component!");
	return this;
}

void AutoShooting::shoot(Game::Direction dir) {
	if (latestShot != nullptr)
		delete latestShot;
	latestShot = shooting->shoot(dir);
}

void AutoShooting::shoot(double angle) {
	if (latestShot != nullptr)
		delete latestShot;
	latestShot = shooting->shoot(angle);
}

std::unique_ptr<Game::Bullet> AutoShooting::pollShot() {
	std::unique_ptr<Game::Bullet> ls(latestShot);
	latestShot = nullptr;
	return ls;
}
