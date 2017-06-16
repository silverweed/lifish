#include <exception>
#include "AutoShooting.hpp"
#include "Shooting.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"

using lif::AutoShooting;

AutoShooting::AutoShooting(lif::Entity& owner)
	: lif::BufferedSpawner(owner)
{
	_declComponent<AutoShooting>();
}

lif::Entity* AutoShooting::init() {
	lif::BufferedSpawner::init();
	shooting = owner.get<Shooting>();
	if (shooting == nullptr)
		throw std::logic_error("owner of AutoShooting has no Shooting component!");
	return this;
}

void AutoShooting::shoot(const sf::Vector2f& target) {
	addSpawned(shooting->shoot(target));
}

void AutoShooting::shoot(lif::Direction dir) {
	addSpawned(shooting->shoot(dir));
}

void AutoShooting::shoot(lif::Angle angle) {
	addSpawned(shooting->shoot(angle));
}
