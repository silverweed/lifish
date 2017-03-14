#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "FreeSighted.hpp"

using lif::ShootingPoint;

ShootingPoint::ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius)
	: lif::Spawning(owner)
{
	_declComponent<ShootingPoint>();
	addComponent<lif::FreeSighted>(*this, visionRadius);
	addComponent<lif::Shooting>(*this, attack);
	autoShooting = addComponent<lif::AutoShooting>(*this);
}

bool ShootingPoint::shouldSpawn() const {
	return autoShooting->shouldSpawn();
}

std::unique_ptr<lif::Entity> ShootingPoint::spawn() {
	return autoShooting->spawn();
}
