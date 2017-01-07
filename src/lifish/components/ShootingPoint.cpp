#include "ShootingPoint.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "FreeSighted.hpp"

using lif::ShootingPoint;

ShootingPoint::ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius)
	: lif::Component(owner)
{
	sighted = addComponent(new lif::FreeSighted(*this, visionRadius));
	shooting = addComponent(new lif::Shooting(*this, attack));
}
