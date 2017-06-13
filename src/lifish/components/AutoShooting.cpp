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

void AutoShooting::shoot(lif::Direction dir, const lif::Entity *const target) {
	addSpawned(shooting->shoot(dir, target));
}

void AutoShooting::shoot(lif::Angle angle, const lif::Entity *const target) {
	addSpawned(shooting->shoot(angle, target));
}
