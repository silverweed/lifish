#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "FreeSighted.hpp"

using lif::ShootingPoint;

ShootingPoint::ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius)
	: lif::Spawning(owner)
{
	_declComponent<ShootingPoint>();
	addComponent<lif::FreeSighted>(*this, visionRadius);
	shooting = addComponent<lif::Shooting>(*this, attack);
}

bool ShootingPoint::shouldSpawn() const {
	return shooting->shouldSpawn();
}

std::unique_ptr<lif::Entity> ShootingPoint::spawn() {
	return shooting->spawn();
}
