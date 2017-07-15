#include "RexBoss.hpp"
#include "LeapingMovement.hpp"
#include "AxisMoving.hpp"
#include "LevelManager.hpp"
#include "Clock.hpp"
#include "Enemy.hpp"
#include "Scored.hpp"
#include "Bonusable.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "Lifed.hpp"
#include "FreeSighted.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "AI.hpp"
#include "Player.hpp"
#include "MovingAnimator.hpp"
#include "BufferedSpawner.hpp"
#include "RexFlame.hpp"
#include "SmokeRing.hpp"
#include "ai_functions.hpp"
#include "ai_helpers.hpp"
#include "conf/boss.hpp"
#include "core.hpp"
#include "camera_utils.hpp"
#include <cassert>
#include <random>

using lif::RexBoss;
using lif::TILE_SIZE;
using namespace lif::conf::boss::rex_boss;

constexpr auto MIN_STEPS = 4;
constexpr auto MAX_STEPS_BEFORE_ATK = MIN_STEPS;
constexpr auto MIN_STEPS_BEFORE_ATK = 2;

lif::AIBoundFunction ai_rex(lif::Entity&);

RexBoss::RexBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	const sf::Vector2f size(4 * TILE_SIZE, 4 * TILE_SIZE);
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::boss::rex_boss::SPEED * lif::Enemy::BASE_SPEED,
			lif::Direction::NONE);
	addComponent<lif::AI>(*this, ai_rex);
	addComponent<lif::LeapingMovement>(*this, sf::seconds(1.5));
	addComponent<lif::Lifed>(*this, LIFE);
	addComponent<lif::Scored>(*this, VALUE);
	animClock = addComponent<lif::Clock>(*this);
	attackClock = addComponent<lif::Clock>(*this);
	sighted = addComponent<lif::FreeSighted>(*this);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("sounds", std::string("rex_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("sounds", std::string("rex_hurt.ogg")))
	});
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "rex_boss.png"));
	animated->addAnimation("start", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_up", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_down", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_left", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("walk_right", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("start", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("death", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("stomp_windup", { sf::IntRect(0, size.y, size.x, size.y) });
	animated->addAnimation("stomp_damage", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("stomp_recover", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("flame_windup", { sf::IntRect(0, size.y, size.x, size.y) });
	animated->addAnimation("flame_damage", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("flame_recover", { sf::IntRect(0, 0, size.x, size.y) });
	animated->setAnimation("start");
	animated->getSprite().setLooped(false, false);
	animated->getSprite().play();
	addComponent<lif::MovingAnimator>(*this)->setActive(false);
	addComponent<lif::BufferedSpawner>(*this);

	// Body collider
	_addDefaultCollider(size);

	// Colliders for attacks
	stompCollider = addComponent<lif::Collider>(*this, [] (lif::Collider& cld) {
		if (cld.getLayer() != lif::c_layers::PLAYERS)
			return;
		auto& player = static_cast<lif::Player&>(cld.getOwnerRW());
		if (!player.get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD))
			player.dealDamage(STOMP_DAMAGE);
	}, lif::c_layers::BOSSES, sf::Vector2f(
		// size
		collider->getRect().width + 2 * STOMP_TILE_RADIUS * TILE_SIZE,
		collider->getRect().height + 2 * STOMP_TILE_RADIUS * TILE_SIZE
	), sf::Vector2f(
		// offset
		-STOMP_TILE_RADIUS * TILE_SIZE,
		-STOMP_TILE_RADIUS * TILE_SIZE
	));
	stompCollider->setActive(false);
}

void RexBoss::update() {
	lif::Entity::update();

	if (killable->isKilled())
		return;

	switch (state) {
	case State::START:
		_updateStart();
		break;
	case State::WALKING:
		if (isAligned() && moving->getPrevAlign() != lif::tile(position))
			++steps;
		_updateWalking();
		break;
	case State::ATTACKING:
		_updateAttacking();
		break;
	case State::DYING:
		_updateDying();
		break;
	default:
		break;
	}
}

void RexBoss::_updateStart() {
	if (animClock->getElapsedTime() >= sf::seconds(2)) {
		animated->getSprite().setLooped(true);
		animated->getSprite().play();
		state = State::WALKING;
		moving->setDirection(lif::Direction::LEFT);
		get<lif::MovingAnimator>()->setActive(true);
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

	const auto dir = moving->getDirection();
	if (dir != lif::Direction::NONE)
		flameDirection = dir;

	// Check state transition
	auto atkCond = _checkAttackCondition();
	if (atkCond >= 0 && atkCond < static_cast<int>(AtkType::N_ATTACKS)) {
		atkType = static_cast<AtkType>(atkCond);
		state = State::ATTACKING;
		atkState = AtkState::ENTERING;
		moving->stop();
		attackClock->restart();
	}
}

void RexBoss::_updateAttacking() {
	std::cout << "attacking: " << static_cast<int>(atkType) << std::endl;
	switch (atkType) {
	case AtkType::STOMP:
		_updateStomp();
		break;
	case AtkType::FLAME:
		_updateFlame();
		break;
	case AtkType::MISSILES:
		_updateMissiles();
		break;
	default:
		break;
	}
	if (atkState == AtkState::EXITING) {
		std::uniform_int_distribution<> dist(0, 3);
		moving->setDirection(lif::ai::directions[dist(lif::rng)]);
		state = State::WALKING;
		std::cout << "=-----\n";
		steps = 0;
	}
}

void RexBoss::_updateStomp() {
	// Task: wind up / damage / recover
	if (atkState == AtkState::ENTERING) {
		atkState = AtkState::WINDUP;
		animated->setAnimation("stomp_windup");
		animated->getSprite().play();
		return;
	}
	const auto time = attackClock->getElapsedTime();
	switch (atkState) {
	case AtkState::WINDUP:
		if (time > STOMP_WINDUP_TIME) {
			atkState = AtkState::DAMAGE;
			animated->setAnimation("stomp_damage");
			animated->getSprite().play();
			// Deal damage
			stompCollider->setActive(true);
			lif::requestCameraShake(0, 0, 0.1, 50, sf::seconds(1), 5);
			const auto rect = collider->getRect();
			auto smoke = new lif::SmokeRing(position + sf::Vector2f(rect.width / 2, rect.height / 2));
			smoke->get<lif::Animated>()->getSprite().setOrigin(TILE_SIZE, TILE_SIZE);
			smoke->get<lif::Animated>()->getSprite().setScale(4, 4);
			get<lif::BufferedSpawner>()->addSpawned(smoke);
			attackClock->restart();
		}
		break;
	case AtkState::DAMAGE:
		if (time > STOMP_DAMAGE_TIME) {
			atkState = AtkState::RECOVER;
			animated->setAnimation("stomp_recover");
			animated->getSprite().play();
			stompCollider->setActive(false);
			attackClock->restart();
		}
		break;
	case AtkState::RECOVER:
		if (time > STOMP_RECOVER_TIME)
			atkState = AtkState::EXITING;
		break;
	default:
		break;
	}
}

void RexBoss::_updateFlame() {
	// Task: wind up / damage / recover
	if (atkState == AtkState::ENTERING) {
		atkState = AtkState::WINDUP;
		animated->setAnimation("flame_windup");
		animated->getSprite().play();
		return;
	}
	const auto time = attackClock->getElapsedTime();
	switch (atkState) {
	case AtkState::WINDUP:
		if (time > FLAME_WINDUP_TIME) {
			atkState = AtkState::DAMAGE;
			animated->setAnimation("flame_damage");
			animated->getSprite().play();
			// Deal damage
			sf::Vector2f flamePos(position);
			const bool isVert = flameDirection == lif::Direction::UP ||
					flameDirection == lif::Direction::DOWN;
			switch (flameDirection) {
			case lif::Direction::LEFT:
				flamePos.x -= FLAME_TILE_WIDTH * TILE_SIZE;
				break;
			case lif::Direction::RIGHT:
				flamePos.x += collider->getRect().width;
				break;
			case lif::Direction::UP:
				flamePos.y -= FLAME_TILE_WIDTH * TILE_SIZE;
				break;
			case lif::Direction::DOWN:
				flamePos.y += collider->getRect().height;
				break;
			default:
				break;
			}
			get<lif::BufferedSpawner>()->addSpawned(new lif::RexFlame(flamePos, isVert
				? sf::Vector2f(FLAME_TILE_HEIGHT * TILE_SIZE, FLAME_TILE_WIDTH * TILE_SIZE)
				: sf::Vector2f(FLAME_TILE_WIDTH * TILE_SIZE, FLAME_TILE_HEIGHT * TILE_SIZE)));
			attackClock->restart();
		}
		break;
	case AtkState::DAMAGE:
		if (time > FLAME_DAMAGE_TIME) {
			atkState = AtkState::RECOVER;
			animated->setAnimation("flame_recover");
			animated->getSprite().play();
			attackClock->restart();
		}
		break;
	case AtkState::RECOVER:
		if (time > FLAME_RECOVER_TIME)
			atkState = AtkState::EXITING;
		break;
	default:
		break;
	}

}

void RexBoss::_updateMissiles() {
	// TODO
}

void RexBoss::_updateDying() {
	if (animated->getAnimationName() != "death") {
		killable->kill();
		animated->setAnimation("death");
		animated->getSprite().setLooped(false);
		animated->getSprite().play();
		lif::requestCameraShake(0.1, 70, 0, 50, sf::seconds(4), 2);
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
	const auto seen = sighted->entitiesSeen();
	const auto lm = get<lif::AI>()->getLevelManager();
	assert(lm && "lm is null in _playersNearby!");
	return std::find_if(seen.begin(), seen.end(), [this, lm] (auto ptr) {
		const auto e = ptr.first.lock();
		return  (lm->isPlayer(*e) && this->_isNearby(*e->template get<lif::Collider>()));
	}) != seen.end();
}

bool RexBoss::_playerAhead() const {
	const auto& seen = sighted->entitiesSeen();
	const auto lm = get<lif::AI>()->getLevelManager();
	assert(lm && "lm is null in _playerAhead!");
	return std::find_if(seen.begin(), seen.end(), [this, lm] (auto ptr) {
		const auto e = ptr.first.lock();
		return lm->isPlayer(*e) && this->_isAhead(*e->template get<lif::Collider>());
	}) != seen.end();
}

bool RexBoss::_isAhead(const lif::Collider& cld) const {
	// Check if `e` is within a 4x3 rectangle ahead of us
	constexpr auto AHEAD_CHECK_WIDTH = 4;
	constexpr auto AHEAD_CHECK_HEIGTH = 4;
	const auto dir = moving->getDirection();
	const bool isVert = dir == lif::Direction::UP || dir == lif::Direction::DOWN;
	sf::FloatRect rect(position.x, position.y,
			(isVert ? AHEAD_CHECK_WIDTH : AHEAD_CHECK_HEIGTH) * TILE_SIZE,
			(isVert ? AHEAD_CHECK_HEIGTH : AHEAD_CHECK_WIDTH) * TILE_SIZE);
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
	sf::FloatRect rect(position.x - NEARBY_CHECK_SIZE * TILE_SIZE,
			position.y - NEARBY_CHECK_SIZE * TILE_SIZE,
			crect.width + 2 * NEARBY_CHECK_SIZE * TILE_SIZE,
			crect.height + 2 * NEARBY_CHECK_SIZE * TILE_SIZE);
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
