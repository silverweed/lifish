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
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "AI.hpp"
#include "ai_functions.hpp"
#include "ai_helpers.hpp"
#include "conf/boss.hpp"
#include "core.hpp"
#include "camera_utils.hpp"

using lif::RexBoss;
using namespace lif::ai;

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
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("sounds", std::string("rex_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("sounds", std::string("rex_hurt.ogg")))
	});
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "rex.png"));
	animated->addAnimation("move_up", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("move_down", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("move_left", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("move_right", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("start", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("death", { sf::IntRect(0, 0, size.x, size.y) });
	animated->getSprite().stop();
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
		_updateWalking();
		break;
	case State::SHOOTING:
		_updateShooting();
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
}

void RexBoss::_updateShootingFlame() {

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

namespace {

struct AIRexFunction {
	lif::Entity& entity;
	lif::AxisMoving *const moving;
	lif::Collider *const collider;
	int consecutive;

	AIRexFunction(lif::Entity& entity, lif::AxisMoving *moving, lif::Collider *collider)
		: entity(entity), moving(moving), collider(collider), consecutive(0)
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
		if (consecutive > 4) {
			collider->reset();
			consecutive = 0;
			NEW_DIRECTION(select_random_viable(*moving, lm, opp, true))
		} else {
			if (moving->getDistTravelled() > 0) ++consecutive;
			SAME_DIRECTION
		}
	}
};

}

// A revisited ai_random_forward, where the entity always changes direction after 4 steps ahead.
lif::AIBoundFunction ai_rex(lif::Entity& entity) {
	constexpr auto MIN_TRAVEL_DISTANCE = lif::TILE_SIZE * 4;

	auto moving = entity.get<lif::AxisMoving>();
	const auto collider = entity.get<lif::Collider>();
	if (moving == nullptr || collider == nullptr)
		throw std::invalid_argument("Entity passed to ai_rex has no Moving or Collider component!");
	moving->setAutoRealign(false);
	moving->setDistTravelled(MIN_TRAVEL_DISTANCE + 1);

	return AIRexFunction(entity, moving, collider);
}
