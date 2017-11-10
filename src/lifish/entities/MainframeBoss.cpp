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
#include "conf/boss.hpp"
#include <cassert>
#include <random>

#define BIND(f) std::bind(&MainframeBoss:: f, this)

using lif::MainframeBoss;
using lif::TILE_SIZE;
using StateFunction = lif::ai::StateFunction;

static const sf::Vector2f SIZE(5 * TILE_SIZE, 5 * TILE_SIZE);
static const sf::Time ATTACK_DELAY = sf::seconds(3);

MainframeBoss::MainframeBoss(const sf::Vector2f& pos, const lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "mainframe_boss.png"));
	animated->setFrameTime("idle", sf::seconds(0.10));
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
	addComponent<lif::Lifed>(*this, lif::conf::boss::mainframe_boss::LIFE);
	addComponent<lif::Scored>(*this, lif::conf::boss::mainframe_boss::VALUE);
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
	if (clock->getElapsedTime() > ATTACK_DELAY) {
		// Choose attack
		static std::uniform_int_distribution<> dist(0, static_cast<int>(AttackType::N_ATTACKS) - 1);
		auto atkType = dist(lif::rng);
		switch (static_cast<AttackType>(atkType)) {
		case AttackType::ROTATING_SURGE:
		default: // TODO
			return BIND(_updateRotatingSurge);
		}
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateRotatingSurge() {
	spawner->addSpawned(new lif::Surge(position + SIZE / 2.f, lif::degrees(5), lm));
	clock->restart();
	return BIND(_updateIdle);
	return std::move(stateFunction);
}
