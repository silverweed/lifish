#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "FreeSighted.hpp"

using lif::ShootingPoint;

ShootingPoint::ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius)
	: lif::Spawning(owner)
{
	keys.emplace_back(_getKey<ShootingPoint>());
	addComponent(std::make_shared<lif::FreeSighted>(*this, visionRadius));
	addComponent(std::make_shared<lif::Shooting>(*this, attack));
	autoShooting = addComponent(std::make_shared<lif::AutoShooting>(*this));
}

bool ShootingPoint::shouldSpawn() const {
	return autoShooting->shouldSpawn();
}

std::unique_ptr<lif::Entity> ShootingPoint::spawn() {
	return autoShooting->spawn();
}
