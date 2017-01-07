#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "FreeSighted.hpp"

using lif::ShootingPoint;

ShootingPoint::ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius)
	: lif::Spawning(owner)
{
	addComponent(new lif::FreeSighted(*this, visionRadius));
	addComponent(new lif::Shooting(*this, attack));
	autoShooting = addComponent(new lif::AutoShooting(*this));
}

bool ShootingPoint::shouldSpawn() const {
	return autoShooting->shouldSpawn();
}

std::unique_ptr<lif::Entity> ShootingPoint::spawn() {
	return autoShooting->spawn();
}
