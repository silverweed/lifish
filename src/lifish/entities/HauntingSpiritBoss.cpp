#include "HauntingSpiritBoss.hpp"
#include "HauntedStatue.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Clock.hpp"
#include "FreeSighted.hpp"
#include "CircleShootingPattern.hpp"
#include "ScatterVsPlayerPattern.hpp"
#include "Killable.hpp"
#include "Scored.hpp"
#include "Angle.hpp"
#include "conf/boss.hpp"
#include "core.hpp"
#include <algorithm>
#include "camera_utils.hpp"
#include <cassert>

#define BIND(f) std::bind(&HauntingSpiritBoss:: f, this)

using lif::HauntingSpiritBoss;
using StateFunction = lif::ai::StateFunction;

HauntingSpiritBoss::HauntingSpiritBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	// This boss has no Lifed component: it dies when there are no HauntedStatues left in the level.
	addComponent<lif::FreeSighted>(*this)->setActive(false);
	addComponent<lif::Scored>(*this, lif::conf::boss::haunting_spirit_boss::VALUE);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "haunting_spirit_boss.png"));
	const auto size = 4 * lif::TILE_SIZE;
	// This is needed by parent class
	collider = addComponent<lif::Collider>(*this, lif::c_layers::DEFAULT,
				sf::Vector2f(size, size),
				sf::Vector2f(-size/2, -size/2), true);
	collider->setActive(false);
	animated->addAnimation("idle", {
		sf::IntRect(0 * size, 0, size, size),
		sf::IntRect(1 * size, 0, size, size),
		sf::IntRect(2 * size, 0, size, size),
		sf::IntRect(3 * size, 0, size, size),
		sf::IntRect(4 * size, 0, size, size),
		sf::IntRect(5 * size, 0, size, size)
	}, true);
	animated->addAnimation("start", {
		sf::IntRect(0 * size, size, size, size),
		sf::IntRect(1 * size, size, size, size),
		sf::IntRect(2 * size, size, size, size),
		sf::IntRect(3 * size, size, size, size),
		sf::IntRect(4 * size, size, size, size),
		sf::IntRect(5 * size, size, size, size)
	});
	animated->addAnimation("transition", {
		sf::IntRect(0 * size, 2 * size, size, size),
		sf::IntRect(1 * size, 2 * size, size, size),
		sf::IntRect(2 * size, 2 * size, size, size),
	});
	animated->addAnimation("death", {
		sf::IntRect(0 * size, 3 * size, size, size),
		sf::IntRect(1 * size, 3 * size, size, size),
		sf::IntRect(2 * size, 3 * size, size, size),
		sf::IntRect(3 * size, 3 * size, size, size),
		sf::IntRect(4 * size, 3 * size, size, size),
		sf::IntRect(5 * size, 3 * size, size, size),
	});
	animated->getSprite().setOrigin(size/2, size/2);
	animClock = addComponent<lif::Clock>(*this);
	hauntClock = addComponent<lif::Clock>(*this);
	atkClock = addComponent<lif::Clock>(*this);

	auto circle = addComponent<lif::CircleShootingPattern>(*this, 102);
	circle->consecutiveShots = 6;
	circle->timeBetweenShots = sf::seconds(0.5);
	circle->bulletsPerShot = 6;
	circle->rotationPerShot = lif::radians(lif::PI / 5.);
	shootPatterns[0] = circle;
	shootColors[0] = sf::Color::White;
	auto spiral = addComponent<lif::CircleShootingPattern>(*this, 102);
	spiral->consecutiveShots = 50;
	spiral->timeBetweenShots = sf::seconds(0.1);
	spiral->bulletsPerShot = 1;
	spiral->rotationPerShot = lif::radians(0.4);
	spiral->randomizeShootAngle = true;
	shootPatterns[1] = spiral;
	shootColors[1] = sf::Color::Blue;
	auto scatter = addComponent<lif::ScatterVsPlayerPattern>(*this, 102);
	scatter->consecutiveShots = 18;
	scatter->timeBetweenShots = sf::seconds(0.1);
	scatter->scatterAngle = lif::degrees(30);
	shootPatterns[2] = scatter;
	shootColors[2] = sf::Color::Red;

	addComponent<lif::Drawable>(*this, *animated);
}

void HauntingSpiritBoss::update() {
	lif::Entity::update();

	assert(stateFunction);
	stateFunction = std::move(stateFunction());
}

StateFunction HauntingSpiritBoss::_updateStart() {
	// Task: play initial animation after a delay
	if (animClock->getElapsedTime() < sf::seconds(2))
		return std::move(stateFunction);

	if (!animated->getSprite().isPlaying()) {
		animated->getSprite().setLooped(true);
		animated->setAnimation("idle");
		animated->getSprite().play();
		get<lif::FreeSighted>()->setActive(true);
		return BIND(_updateSearching);

	} else if (animated->getAnimationName() == "idle") {
		animated->getSprite().setLooped(false, false);
		animated->setAnimation("start");
		animated->getSprite().play();
	}

	return std::move(stateFunction);
}

StateFunction HauntingSpiritBoss::_updateSearching() {
	// Task: find all the HauntedStatues in the level.
	// Note: since FreeSighted returns raw pointers but we want to keep track of the living state of
	// the statues, we track the *weak pointers* to their Killable component. This will expire together
	// with the statue itself.
	auto sighted = get<lif::FreeSighted>();
	const auto& seen = sighted->entitiesSeen();
	for (const auto& pair : seen) {
		if (auto statue = dynamic_cast<const lif::HauntedStatue*>(pair.first))
			if (!statue->isPossessed())
				statues.emplace_back(statue->getShared<lif::Killable>());
	}
	sighted->setActive(false); // no need for this anymore.
	return BIND(_updateSelectNewStatue);
}

StateFunction HauntingSpiritBoss::_updateSelectNewStatue() {
	// Task: select a statue to possess
	for (auto it = statues.begin(); it != statues.end(); ) {
		if (it->expired())
			it = statues.erase(it);
		else
			++it;
	}
	if (statues.size() == 0) {
		return BIND(_updateDying);
	}
	std::uniform_int_distribution<> dist(0, statues.size() - 1);
	targetStatue = statues[dist(lif::rng)];
	return BIND(_updateTransitioningBegin);
}

StateFunction HauntingSpiritBoss::_updateTransitioningBegin() {
	// Task: play the transitioning animation and set the next haunted statue as `possessed`
	if (animated->getAnimationName() != "transition") {
		animated->setAnimation("transition");
		animated->getSprite().setLooped(false, false);
		animated->getSprite().play();
		animClock->restart();
	}
	if (position.y < 0) {
		// Center on the target statue
		const auto statue = targetStatue.lock();
		if (statue == nullptr) {
			return BIND(_updateDying);
		}
		position.x = statue->getOwner().getPosition().x + lif::TILE_SIZE / 2;
		animated->getSprite().rotate(180);
		return BIND(_updateTransitioningEnd);
	} else {
		position.y -= animClock->restart().asSeconds() * 200;
	}
	return std::move(stateFunction);
}

StateFunction HauntingSpiritBoss::_updateTransitioningEnd() {
	const auto statue = targetStatue.lock();
	if (statue == nullptr) {
		return BIND(_updateSelectNewStatue);
	}
	if (position.y >= statue->getOwner().getPosition().y) {
		animated->getSprite().rotate(180);
		animated->getSprite().setLooped(true);
		animated->setAnimation("idle");
		animated->getSprite().play();
		hauntClock->restart();
		atkClock->restart();
		static_cast<lif::HauntedStatue&>(targetStatue.lock()->getOwnerRW()).setPossessed(true);
		get<lif::Drawable>()->setActive(false);
		selectedNewPattern = false;
		return BIND(_updateHaunting);
	}

	position.y += animClock->restart().asSeconds() * 200;

	return std::move(stateFunction);
}

StateFunction HauntingSpiritBoss::_updateHaunting() {
	// Task: attack the player; leave after some delay
	if (targetStatue.expired()) {
		get<lif::Drawable>()->setActive(true);
		if (curShootPattern != nullptr)
			curShootPattern->setActive(false);
		return BIND(_updateSelectNewStatue);
	}
	auto& statue = static_cast<lif::HauntedStatue&>(targetStatue.lock()->getOwnerRW());
	if (_isShooting()) {
		atkClock->restart();
		return std::move(stateFunction);
	} else if (!selectedNewPattern) {
		// Select the next pattern as soon as we stop shooting;
		// actually start shooting only after PATTERN_SHOOT_DELAY
		// (the pattern is selected now so we can change the spirit's color)
		selectedNewPattern = true;
		std::uniform_int_distribution<unsigned> dist(0, shootPatterns.size() - 1);
		const auto idx = dist(lif::rng);
		curShootPattern = shootPatterns[idx];
		statue.setSpiritColor(shootColors[idx]);
	}
	// We haunted this statue long enough: select a new one
	if (hauntClock->getElapsedTime() > lif::conf::boss::haunting_spirit_boss::CHANGE_STATUE_DELAY) {
		get<lif::Drawable>()->setActive(true);
		statue.setPossessed(false);
		return BIND(_updateSelectNewStatue);;
	}
	if (atkClock->getElapsedTime() > lif::conf::boss::haunting_spirit_boss::PATTERN_SHOOT_DELAY) {
		curShootPattern->resetAndPlay();
		selectedNewPattern = false;
	}
	return std::move(stateFunction);
}

StateFunction HauntingSpiritBoss::_updateDying() {
	// Task: play death animation
	if (animated->getAnimationName() != "death") {
		killable->kill();
		animated->setAnimation("death");
		animated->getSprite().setLooped(false);
		animated->getSprite().play();
		lif::requestCameraShake(0.06, 100, 0, 0, sf::seconds(4), 2);
	}
	return std::move(stateFunction);
}

bool HauntingSpiritBoss::_isShooting() const {
	return curShootPattern != nullptr && curShootPattern->isActive();
}
