#include "HauntingSpiritBoss.hpp"
#include "Angle.hpp"
#include "Animated.hpp"
#include "CircleShootingPattern.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "FreeSighted.hpp"
#include "HauntedStatue.hpp"
#include "Killable.hpp"
#include "OneShotFX.hpp"
#include "ScatterVsPlayerPattern.hpp"
#include "Scored.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "camera_utils.hpp"
#include "conf/boss.hpp"
#include "conf/zindex.hpp"
#include "core.hpp"
#include <algorithm>
#include <cassert>

#define BIND(f) std::bind(&HauntingSpiritBoss:: f, this)

using lif::HauntingSpiritBoss;
using StateFunction = lif::ai::StateFunction;
using namespace lif::conf::boss::haunting_spirit_boss;

const auto BOSS_SIZE = 4 * lif::TILE_SIZE;

HauntingSpiritBoss::HauntingSpiritBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	get<lif::ZIndexed>()->setZIndex(lif::conf::zindex::HAUNTING_SPIRIT_BOSS);
	// This boss has no Lifed component: it dies when there are no HauntedStatues left in the level.
	addComponent<lif::FreeSighted>(*this);
	addComponent<lif::Scored>(*this, VALUE);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "haunting_spirit_boss.png"));
	spawner = addComponent<lif::BufferedSpawner>(*this);
	// This is needed by parent class
	collider = addComponent<lif::Collider>(*this, lif::c_layers::DEFAULT,
				sf::Vector2f(BOSS_SIZE, BOSS_SIZE),
				sf::Vector2f(-BOSS_SIZE/2, -BOSS_SIZE/2), true);
	collider->setActive(false);
	animated->addAnimation("idle", {
		sf::IntRect(0 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(1 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(2 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(3 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(4 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(5 * BOSS_SIZE, 0, BOSS_SIZE, BOSS_SIZE)
	}, true);
	animated->addAnimation("start", {
		sf::IntRect(0 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(1 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(2 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(4 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(5 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE, BOSS_SIZE)
	});
	animated->addAnimation("transition", {
		sf::IntRect(0 * BOSS_SIZE, 2 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(1 * BOSS_SIZE, 2 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(2 * BOSS_SIZE, 2 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
	});
	animated->addAnimation("death", {
		sf::IntRect(0 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(1 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(2 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(3 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(4 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
		sf::IntRect(5 * BOSS_SIZE, 3 * BOSS_SIZE, BOSS_SIZE, BOSS_SIZE),
	});
	animated->getSprite().setOrigin(BOSS_SIZE/2, BOSS_SIZE/2);

	auto circle = addComponent<lif::CircleShootingPattern>(*this, 102);
	circle->consecutiveShots = 6;
	circle->timeBetweenShots = sf::seconds(0.5);
	circle->bulletsPerShot = 6;
	circle->rotationPerShot = lif::radians(lif::PI / 5.);
	shootPatterns[0] = circle;
	shootColors[0] = sf::Color(191, 15, 255);
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
	lif::Boss::update();

	const auto delta = lif::time.getDelta();
	atkT += delta;
	animT += delta;
	hauntT += delta;

	assert(stateFunction);
	stateFunction = stateFunction();
}

StateFunction HauntingSpiritBoss::_updateStart() {
	// Task: play initial animation after a delay
	if (animT < sf::seconds(2))
		return std::move(stateFunction);

	if (!animated->getSprite().isPlaying()) {
		animated->getSprite().setLooped(true);
		animated->setAnimation("idle");
		animated->getSprite().play();
		return BIND(_updateSearching);

	} else if (animated->getAnimationName() == lif::sid("idle")) {
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
	return BIND(_updateSelectNewStatue);
}

StateFunction HauntingSpiritBoss::_updateSelectNewStatue() {
	// Task: select a statue to possess
	statues.erase(std::remove_if(statues.begin(), statues.end(), [] (auto it) {
		return it.expired();
	}), statues.end());

	if (statues.size() == 0) {
		return BIND(_updateDying);
	}
	std::shuffle(statues.begin(), statues.end(), lif::rng);
	// Among the shuffled statues, try to select one in the opposite row than the current.
	for (auto it = statues.begin(); it != statues.end(); ++it) {
		assert(!it->expired());
		if (std::abs(it->lock()->getOwner().getPosition().x - position.x) > lif::TILE_SIZE) {
			targetStatue = *it;
			break;
		}
	}
	if (targetStatue.expired())
		targetStatue = statues[0];

	return BIND(_updateTransitioningBegin);
}

StateFunction HauntingSpiritBoss::_updateTransitioningBegin() {
	// Task: play the transitioning animation and set the next haunted statue as `possessed`
	if (animated->getAnimationName() != lif::sid("transition")) {
		animated->setAnimation("transition");
		animated->getSprite().setLooped(false, false);
		animated->getSprite().play();
		animT = sf::Time::Zero;
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
		position.y -= animT.asSeconds() * 200;
		animT = sf::Time::Zero;
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
		hauntT = sf::Time::Zero;
		atkT = sf::Time::Zero;
		static_cast<lif::HauntedStatue&>(targetStatue.lock()->getOwnerRW()).setPossessed(true);
		get<lif::Drawable>()->setActive(false);
		selectedNewPattern = false;
		return BIND(_updateHaunting);
	}

	position.y += animT.asSeconds() * 200;
	animT = sf::Time::Zero;

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
		atkT = sf::Time::Zero;
		return std::move(stateFunction);
	} else if (!selectedNewPattern) {
		// Select the next pattern as soon as we stop shooting;
		// actually start shooting only after PATTERN_SHOOT_DELAY
		// (the pattern is selected now so we can change the spirit's color)
		selectedNewPattern = true;
		showedAtkCue = false;
		std::uniform_int_distribution<unsigned> dist(0, shootPatterns.size() - 1);
		curShootIdx = dist(lif::rng);
		curShootPattern = shootPatterns[curShootIdx];
		statue.setSpiritColor(shootColors[curShootIdx]);
	}
	// We haunted this statue long enough: select a new one
	if (hauntT > CHANGE_STATUE_DELAY) {
		get<lif::Drawable>()->setActive(true);
		statue.setPossessed(false);
		return BIND(_updateSelectNewStatue);
	}
	// Waiting for next attack
	if (atkT > PATTERN_SHOOT_DELAY) {
		curShootPattern->resetAndPlay();
		selectedNewPattern = false;
	} else if (!showedAtkCue && atkT > PATTERN_SHOOT_DELAY - sf::seconds(0.75)) {
		// Give a visual cue of the incoming attack
		auto blink = new lif::OneShotFX(position - sf::Vector2f(BOSS_SIZE * 0.25, BOSS_SIZE * 0.25),
			"blink.png", {
				sf::IntRect(0 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
				sf::IntRect(1 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
				sf::IntRect(2 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
				sf::IntRect(3 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
				sf::IntRect(4 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			});
		blink->get<lif::Animated>()->getSprite().setColor(shootColors[curShootIdx]);
		spawner->addSpawned(blink);
		showedAtkCue = true;
	}
	return std::move(stateFunction);
}

StateFunction HauntingSpiritBoss::_updateDying() {
	// Task: play death animation
	if (animated->getAnimationName() != lif::sid("death")) {
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
