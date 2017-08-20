#include "LeapingMovement.hpp"
#include "AxisMoving.hpp"
#include "utils.hpp"
#include <random>

using lif::LeapingMovement;

LeapingMovement::LeapingMovement(lif::Entity& owner, sf::Time blockTime)
	: lif::Component(owner)
	, blockTime(blockTime)
{
	_declComponent<LeapingMovement>();
}

lif::Entity* LeapingMovement::init() {
	// moving should have ensureAlign = true
	moving = owner.get<lif::AxisMoving>();
	if (moving == nullptr)
		throw std::invalid_argument("Owner of LeapingMovement has no AxisMoving!");

	std::uniform_real_distribution<float> dist(0, blockTime.asSeconds());
	moving->block(sf::seconds(dist(lif::rng)));

	return this;
}

void LeapingMovement::update() {
	lif::Component::update();

	if (!moving->isBlocked()) {
		// This prevents some AI malfunctioning, e.g. ai_random_forward.
		moving->setDistTravelled(0);
		moving->setSpeed(1, true);
	}

	if (owner.isAligned() && lif::tile(owner.getPosition()) != moving->getPrevAlign()) {
		moving->block(blockTime);
		moving->realign();
		// Prevent the entity to misalign as soon as it exits block, or the AI will not be updated.
		moving->setSpeed(0);
	}
}
