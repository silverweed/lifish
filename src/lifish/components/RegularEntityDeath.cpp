#include "RegularEntityDeath.hpp"
#include "Sounded.hpp"
#include "MovingAnimator.hpp"
#include "GameCache.hpp"
#include "game.hpp"
#include <exception>

using Game::RegularEntityDeath;

RegularEntityDeath::RegularEntityDeath(Game::Entity& owner, sf::Time deathTime)
	: Game::Component(owner)
	, deathTime(deathTime)
{
	killable = owner.get<Game::Killable>();
	if (killable == nullptr)
		throw std::invalid_argument("RegularEntityDeath's owner has no Killable component!");
}

void RegularEntityDeath::kill() {
	auto moving = owner.get<Game::AxisMoving>();
	if (moving != nullptr) {
		moving->setAutoRealignEnabled(false);
		moving->stop();
		auto movingAnimator = owner.get<Game::MovingAnimator>();
		if (movingAnimator != nullptr)
			movingAnimator->setActive(false);
	}
	auto animated = owner.get<Game::Animated>();
	if (animated != nullptr) {
		auto& animatedSprite = animated->getSprite();
		animated->setAnimation("death");
		animatedSprite.play();
	}
	auto sounded = owner.get<Game::Sounded>();
	if (sounded != nullptr)
		Game::cache.playSound(sounded->getSoundFile(Game::Sounds::DEATH));
}

bool RegularEntityDeath::isKillInProgress() const {
	return killable->timeSinceDeath() < deathTime;
}
