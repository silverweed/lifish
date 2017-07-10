#include "RexBoss.hpp"
#include "LeapingMovement.hpp"
#include "AxisMoving.hpp"
#include "LevelManager.hpp"
#include "Enemy.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
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
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::boss::rex_boss::SPEED * lif::Enemy::BASE_SPEED,
			lif::Direction::LEFT);
	addComponent<lif::AI>(*this, ai_rex);
	addComponent<lif::LeapingMovement>(*this, sf::seconds(1.5));
	const sf::Vector2f size(4 * lif::TILE_SIZE, 4 * lif::TILE_SIZE);
	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, lif::c_layers::BOSSES, size);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "rex.png"));
	animated->addAnimation("move_up", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("move_down", { sf::IntRect(0, 0, size.x, size.y) });
	animated->addAnimation("move_left", { sf::IntRect(0, 0, size.x, size.y) }, true);
	animated->addAnimation("move_right", { sf::IntRect(0, 0, size.x, size.y) });
}

void RexBoss::update() {
	lif::Entity::update();

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
