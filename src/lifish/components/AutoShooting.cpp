#include <exception>
#include "AutoShooting.hpp"
#include "Shooting.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include <iostream>

using lif::AutoShooting;

AutoShooting::AutoShooting(lif::Entity& owner)
	: lif::BufferedSpawner(owner)
{}

lif::Entity* AutoShooting::init() {
	lif::BufferedSpawner::init();
	shooting = owner.get<Shooting>();
	std::cerr << "my owner is " << owner.toString() << std::endl;
	if (shooting == nullptr)
		throw std::logic_error("owner of AutoShooting has no Shooting component!");
	return this;
}

void AutoShooting::shoot(lif::Direction dir) {
	addSpawned(shooting->shoot(dir));
}

void AutoShooting::shoot(lif::Angle angle) {
	// FIXME
	addSpawned(shooting->shoot(angle));
}
