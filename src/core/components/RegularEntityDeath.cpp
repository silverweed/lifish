#include "RegularEntityDeath.hpp"
#include "Sounded.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Killable.hpp"
#include "MovingAnimator.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "GameCache.hpp"
#include "core.hpp"
#include <exception>

using Game::RegularEntityDeath;

RegularEntityDeath::RegularEntityDeath(Game::Entity& owner, sf::Time deathTime)
	: Game::Component(owner)
	, deathTime(deathTime)
{}

Game::Entity* RegularEntityDeath::init() {
	killable = owner.get<Game::Killable>();
	if (killable == nullptr)
		throw std::invalid_argument("RegularEntityDeath's owner has no Killable component!");
	return this;
}

void RegularEntityDeath::kill() {
	// Stop all moving components
	auto moving = owner.getAllRecursive<Game::AxisMoving>();
	for (auto mv : moving) {
		origAutoRealign[mv] = mv->isAutoRealignEnabled();
		mv->setAutoRealign(false);
		mv->stop();
	}
	
	// Disable all MovingAnimators
	auto movingAnimators = owner.getAllRecursive<Game::MovingAnimator>();
	for (auto movingAnimator : movingAnimators)
		movingAnimator->setActive(false);

	// Switch to death animation
	auto animated = owner.getAllRecursive<Game::Animated>();
	for (auto anim : animated) {
		if (!anim->hasAnimation("death")) continue;
		auto& animSprite = anim->getSprite();
		anim->setAnimation("death");
		animSprite.play();
	}

	auto colliders = owner.getAllRecursive<Game::Collider>();
	for (auto cld : colliders)
		cld->reset();

	// Play death sound
	auto sounded = owner.get<Game::Sounded>();
	if (sounded != nullptr)
		Game::cache.playSound(sounded->getSoundFile(Game::Sounds::DEATH));
}

void RegularEntityDeath::resurrect() {
	// Reset all moving components
	auto moving = owner.getAllRecursive<Game::AxisMoving>();
	for (auto mv : moving) {
		auto ar = origAutoRealign.find(mv);
		if (ar != origAutoRealign.end())
			mv->setAutoRealign(ar->second);
	}
	
	// Reset all MovingAnimators
	auto movingAnimators = owner.getAllRecursive<Game::MovingAnimator>();
	for (auto movingAnimator : movingAnimators)
		movingAnimator->setActive(true);

	// Switch to idle animation
	auto animated = owner.getAllRecursive<Game::Animated>();
	for (auto anim : animated) {
		if (!anim->hasAnimation("death")) continue;
		auto& animSprite = anim->getSprite();
		anim->setAnimation("idle");
		animSprite.play();
	}
}

bool RegularEntityDeath::isKillInProgress() const {
	return killable->timeSinceDeath() < deathTime;
}
