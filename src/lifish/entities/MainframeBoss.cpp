#include "MainframeBoss.hpp"
#include "game.hpp"
#include "Animated.hpp"
#include "BufferedSpawner.hpp"
#include "Clock.hpp"
#include "Killable.hpp"
#include "HurtDrawProxy.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"
#include "Scored.hpp"
#include "Surge.hpp"
#include "SurgeWarn.hpp"
#include "conf/boss.hpp"
#include <cassert>
#include <random>

#define BIND(f) std::bind(&MainframeBoss:: f, this)

using lif::MainframeBoss;
using lif::TILE_SIZE;
using StateFunction = lif::ai::StateFunction;
using namespace lif::conf::boss::mainframe_boss;

static const sf::Vector2f SIZE(5 * TILE_SIZE, 5 * TILE_SIZE);
static const sf::Time IDLE_FRAME_TIME = sf::milliseconds(100);
static std::uniform_real_distribution<float> angleDist(0, 360);

MainframeBoss::MainframeBoss(const sf::Vector2f& pos, const lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "mainframe_boss.png"));
	animated->setFrameTime("idle", IDLE_FRAME_TIME);
	animated->addAnimation("idle", {
		sf::IntRect(0 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(4 * SIZE.x, 0, SIZE.x, SIZE.y),
	}, true);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("sounds", std::string("mainframe_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("sounds", std::string("mainframe_hurt.ogg")))
	});
	addComponent<lif::Lifed>(*this, LIFE);
	addComponent<lif::Scored>(*this, VALUE); 
	clock = addComponent<lif::Clock>(*this);
	spawner = addComponent<lif::BufferedSpawner>(*this);

	addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));
	_addDefaultCollider(SIZE);
}

void MainframeBoss::update() {
	lif::Entity::update();
	
	if (killable->isKilled())
		return;

	assert(stateFunction);
	stateFunction = stateFunction();
}

StateFunction MainframeBoss::_updateIdle() {
	if (clock->getElapsedTime() > IDLE_TIME) {
		// Choose attack
		static std::uniform_int_distribution<> dist(0, static_cast<int>(AttackType::N_ATTACKS) - 1);
		auto atkType = dist(lif::rng);
		switch (static_cast<AttackType>(atkType)) {
		case AttackType::ROTATING_SURGE:
		default: // TODO
			clock->restart();
			return BIND(_updateSurgeEntering);
		}
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSurgeEntering() {
	animated->setFrameTime("idle", sf::milliseconds(30));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(255, 200, 0));
	nextAttackAngle = lif::degrees(angleDist(lif::rng));
	// spawn visual warning of surge
	spawner->addSpawned(new lif::SurgeWarn(position + SIZE * 0.5f, SURGE_WINDUP_TIME, nextAttackAngle));
	clock->restart();
	return BIND(_updateSurgeWindup);
}

StateFunction MainframeBoss::_updateSurgeWindup() {
	if (clock->getElapsedTime() > SURGE_WINDUP_TIME) {
		animated->setFrameTime("idle", IDLE_FRAME_TIME);
		animated->setAnimation("idle");
		animated->getSprite().setColor(sf::Color::White);
		spawner->addSpawned(new lif::Surge(position + SIZE * 0.5f, lm, nextAttackAngle,
				SURGE_ROT_PER_SECOND, SURGE_SPANNED_ANGLE));
		clock->restart();
		return BIND(_updateSurgeRecover);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSurgeRecover() {
	if (clock->getElapsedTime() > SURGE_RECOVER_TIME) {
		clock->restart();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}
