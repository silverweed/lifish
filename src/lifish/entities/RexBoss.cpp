#include "RexBoss.hpp"
#include "LeapingMovement.hpp"
#include "AxisMoving.hpp"
#include "LevelManager.hpp"
#include "Clock.hpp"
#include "Enemy.hpp"
#include "Scored.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "Lifed.hpp"
#include "FreeSighted.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "AI.hpp"
#include "ai_functions.hpp"
#include "ai_helpers.hpp"
#include "conf/boss.hpp"
#include "core.hpp"
#include "camera_utils.hpp"
#include <cassert>
#include <random>
#include "MovingAnimator.hpp"

using lif::RexBoss;

constexpr auto MIN_STEPS = 4;
constexpr auto MAX_STEPS_BEFORE_ATK = MIN_STEPS;
constexpr auto MIN_STEPS_BEFORE_ATK = 2;
constexpr auto STOMP_DISTANCE = 2 * lif::TILE_SIZE;

lif::AIBoundFunction ai_rex(lif::Entity&);

RexBoss::RexBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	const sf::Vector2f size(4 * lif::TILE_SIZE, 4 * lif::TILE_SIZE);
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::boss::rex_boss::SPEED * lif::Enemy::BASE_SPEED,
			lif::Direction::LEFT);
	addComponent<lif::AI>(*this, ai_rex);
	addComponent<lif::LeapingMovement>(*this, sf::seconds(1.5));
	_addDefaultCollider(size);
	addComponent<lif::Lifed>(*this, lif::conf::boss::rex_boss::LIFE);
	addComponent<lif::Scored>(*this, lif::conf::boss::rex_boss::VALUE);
	animClock = addComponent<lif::Clock>(*this);
	attackClock = addComponent<lif::Clock>(*this);
	sighted = addComponent<lif::FreeSighted>(*this);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("sounds", std::string("rex_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("sounds", std::string("rex_hurt.ogg")))
	});
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "rex_boss.png"));
	animated->addAnimation("walk_up", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_down", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_left", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_right", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("start", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("death", { sf::IntRect(0, 0, size.x, size.y) });
	animated->getSprite().stop();
	addComponent<lif::MovingAnimator>(*this)->setActive(false);
}

void RexBoss::update() {
	lif::Entity::update();

	if (killable->isKilled())
		return;
	
	if (isAligned() && moving->getPrevAlign() != lif::tile(position))
		++steps;

	switch (state) {
	case State::START:
		_updateStart();
		break;
	case State::WALKING:
		_updateWalking();
		break;
	case State::ATTACKING:
		_updateAttacking();
		break;
	default:
		break;
	}
}

void RexBoss::_updateStart() {
	if (animClock->getElapsedTime() < sf::seconds(2)) return;
	if (!animated->getSprite().isPlaying()) {
		animated->getSprite().setLooped(true);
		animated->setAnimation("idle");
		animated->getSprite().play();
		state = State::WALKING;
		get<lif::MovingAnimator>()->setActive(true);
	} else if (animated->getAnimationName() == "idle") {
		animated->getSprite().setLooped(false, false);
		animated->setAnimation("start");
		animated->getSprite().play();
	}
}

void RexBoss::_updateWalking() {
	if (moving->isBlocked()) {
		if (!wasBlocked) {
			// Stomp
			lif::requestCameraShake(0, 0, 0.01, 50, sf::seconds(0.6), 3);
			wasBlocked = true;
		}
	} else {
		wasBlocked = false;
	}

	if (!isAligned()) return;

	// Check state transition
	auto atkCond = _checkAttackCondition();
	if (atkCond >= 0 && atkCond < static_cast<int>(AtkType::N_ATTACKS)) {
		atkType = static_cast<AtkType>(atkCond);
		state = State::ATTACKING;
		moving->stop();
		attackClock->restart();
	}
}

void RexBoss::_updateAttacking() {
	std::cout << "attacking: " << static_cast<int>(atkType) << std::endl;
	if (attackClock->getElapsedTime() > sf::seconds(2)) {
		std::uniform_int_distribution<> dist(0, 3);
		moving->setDirection(lif::ai::directions[dist(lif::rng)]);
		state = State::WALKING;
		std::cout << "=-----\n";
		steps = 0;
	}
}

int RexBoss::_checkAttackCondition() const {
	if (steps < MIN_STEPS_BEFORE_ATK)
		return -1;

	std::vector<AtkType> viable;
	if (_playerAhead())
		viable.emplace_back(AtkType::FLAME);
	if (_playersNearby())
		viable.emplace_back(AtkType::STOMP);
	if (viable.size() < 2 && steps >= MAX_STEPS_BEFORE_ATK)
		viable.emplace_back(AtkType::MISSILES);

	if (viable.size() == 0)
		return -1;

	std::uniform_int_distribution<> dist(0, viable.size() - 1);
	return static_cast<int>(viable[dist(lif::rng)]);
}

void RexBoss::_kill() {
	moving->setActive(false);
	if (animated->getAnimationName() != "death") {
		animated->setAnimation("death");
		animated->getSprite().setLooped(false);
		animated->getSprite().play();
		lif::requestCameraShake(0.06, 70, 0.03, 50, sf::seconds(4), 2);
	}
	lif::Boss::_kill();
}

bool RexBoss::_playersNearby() const {
	const auto lm = get<lif::AI>()->getLevelManager();
	assert(lm && "lm is null in _playersNearby!");
	for (auto ptr : sighted->entitiesSeen()) {
		const auto e = ptr.first.lock();
		if (lm->isPlayer(*e) && _isNearby(*e->get<lif::Collider>()))
			return true;
	}
	return false;
}

bool RexBoss::_playerAhead() const {
	const auto& seen = sighted->entitiesSeen();
	const auto lm = get<lif::AI>()->getLevelManager();
	assert(lm && "lm is null in _playerAhead!");
	return std::find_if(seen.begin(), seen.end(), [this, lm] (auto ptr) {
		const auto e = ptr.first.lock();
		return lm->isPlayer(*e) && this->_isAhead(*e->get<lif::Collider>());
	}) != seen.end();
}

bool RexBoss::_isAhead(const lif::Collider& cld) const {
	// Check if `e` is within a 4x3 rectangle ahead of us
	constexpr auto AHEAD_CHECK_WIDTH = 4;
	constexpr auto AHEAD_CHECK_HEIGTH = 4;
	const auto dir = moving->getDirection();
	const bool isVert = dir == lif::Direction::UP || dir == lif::Direction::DOWN;
	sf::FloatRect rect(position.x, position.y,
			(isVert ? AHEAD_CHECK_WIDTH : AHEAD_CHECK_HEIGTH) * lif::TILE_SIZE,
			(isVert ? AHEAD_CHECK_HEIGTH : AHEAD_CHECK_WIDTH) * lif::TILE_SIZE);
	switch (dir) {
	case lif::Direction::LEFT:
		rect.left -= rect.width;
		break;
	case lif::Direction::RIGHT:
		rect.left += collider->getRect().width;
		break;
	case lif::Direction::UP:
		rect.top -= rect.height;
		break;
	case lif::Direction::DOWN:
		rect.top += collider->getRect().height;
		break;
	default:
		return false;
	}
	return rect.intersects(cld.getRect());
}

bool RexBoss::_isNearby(const lif::Collider& cld) const {
	// Check if `e` is nearer than 2 tiles from our binding box
	constexpr auto NEARBY_CHECK_SIZE = 1;
	const auto crect = collider->getRect();
	sf::FloatRect rect(position.x - NEARBY_CHECK_SIZE * lif::TILE_SIZE,
			position.y - NEARBY_CHECK_SIZE * lif::TILE_SIZE,
			crect.width + 2 * NEARBY_CHECK_SIZE * lif::TILE_SIZE,
			crect.height + 2 * NEARBY_CHECK_SIZE * lif::TILE_SIZE);
	return rect.intersects(cld.getRect());
}

///////////////////////////////////////

namespace {

struct AIRexFunction {
	lif::Entity& entity;
	lif::AxisMoving *const moving;
	lif::Collider *const collider;
	int consecutive;
	const int minSteps;

	AIRexFunction(lif::Entity& entity, lif::AxisMoving *moving, lif::Collider *collider, int minSteps)
		: entity(entity), moving(moving), collider(collider), consecutive(0), minSteps(minSteps)
	{}

	void operator()(const lif::LevelManager& lm) {
		HANDLE_NOT_MOVING;
		HANDLE_UNALIGNED;
		const auto cur = moving->getDirection();
		const bool colliding = collider->collidesWithSolid();
		const auto opp = lif::oppositeDirection(cur);
		// colliding with a moving entity
		if (colliding && lm.canGo(*moving, cur)) {
			consecutive = 0;
			NEW_DIRECTION(opp)
		}
		// Note: this `if` prevents the entity to change direction twice in a row even
		// when it shouldn't (due to the fact that, if its movement is less than 1px/frame,
		// it may be considered "aligned" again after changing direction.)
		if (consecutive > minSteps) {
			collider->reset();
			consecutive = 0;
			NEW_DIRECTION(lif::ai::select_random_viable(*moving, lm, opp, true))
		} else {
			if (moving->getDistTravelled() > 0) ++consecutive;
			SAME_DIRECTION
		}
	}
};

}

// A revisited ai_random_forward, where the entity always changes direction after 4 steps ahead.
lif::AIBoundFunction ai_rex(lif::Entity& entity) {
	auto moving = entity.get<lif::AxisMoving>();
	const auto collider = entity.get<lif::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_rex has no Moving or Collider component!");
	moving->setAutoRealign(false);
	moving->setDistTravelled(1);

	return AIRexFunction(entity, moving, collider, MIN_STEPS);
}
