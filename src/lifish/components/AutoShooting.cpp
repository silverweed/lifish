#include <exception>
#include "AutoShooting.hpp"
#include "Shooting.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include <iostream>

using Game::AutoShooting;

AutoShooting::AutoShooting(Game::Entity& owner)
	: Game::BufferedSpawner(owner)
{}

Game::Entity* AutoShooting::init() {
	Game::BufferedSpawner::init();
	shooting = owner.get<Shooting>();
	std::cerr << "my owner is " << owner.toString() << std::endl;
	if (shooting == nullptr)
		throw std::logic_error("owner of AutoShooting has no Shooting component!");
	return this;
}

void AutoShooting::shoot(Game::Direction dir) {
	addSpawned(shooting->shoot(dir));
}

void AutoShooting::shoot(Game::Angle angle) {
	// FIXME
	addSpawned(shooting->shoot(angle));
}
