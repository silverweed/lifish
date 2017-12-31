#include "MainframeBoss.hpp"
#include "game.hpp"
#include "Animated.hpp"
#include "TimedLaser.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Bonusable.hpp"
#include "BufferedSpawner.hpp"
#include "LightSource.hpp"
#include "OneShotFX.hpp"
#include "Clock.hpp"
#include "Killable.hpp"
#include "Level.hpp"
#include "HurtDrawProxy.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"
#include "Scored.hpp"
#include "Surge.hpp"
#include "SurgeWarn.hpp"
#include "BulletFactory.hpp"
#include "conf/boss.hpp"
#include <cassert>
#include <random>
#include <cstdlib>
#include "DebugPainter.hpp"

#define BIND(f) std::bind(&MainframeBoss:: f, this)

using lif::MainframeBoss;
using lif::TILE_SIZE;
using StateFunction = lif::ai::StateFunction;
using namespace lif::conf::boss::mainframe_boss;

static const sf::Vector2f SIZE(5 * TILE_SIZE, 5 * TILE_SIZE);
static const sf::Time IDLE_FRAME_TIME = sf::milliseconds(100);
static std::uniform_real_distribution<float> angleDist(0, 360);
constexpr auto SHIELD_DIAMETER = 8 * TILE_SIZE;

MainframeBoss::MainframeBoss(const sf::Vector2f& pos, const lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
	, shieldSprite(SHIELD_DIAMETER / 2)
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
	sparkClock = addComponent<lif::Clock>(*this);
	spawner = addComponent<lif::BufferedSpawner>(*this);

	hurtDrawProxy = addComponent<lif::HurtDrawProxy>(*this);
	addComponent<lif::Drawable>(*this, *this);
	_addDefaultCollider(SIZE);

	shieldSprite.setPosition(position.x + SIZE.x * 0.5f, position.y + SIZE.y * 0.5f);
	shieldSprite.setOrigin(SHIELD_DIAMETER / 2, SHIELD_DIAMETER / 2);
	shieldSprite.setFillColor(sf::Color(255, 255, 0, 0));
	// This is only used for flickering the shield.
	lightSource = addComponent<lif::LightSource>(*this, 0, sf::Color::White, 0.6, 6);
}

void MainframeBoss::update() {
	lif::Entity::update();
	
	if (killable->isKilled())
		return;

	assert(stateFunction);
	stateFunction = stateFunction();
}

void MainframeBoss::_resetIdleAnim() {
	animated->setFrameTime("idle", IDLE_FRAME_TIME);
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color::White);
}

void MainframeBoss::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*hurtDrawProxy, states);
	target.draw(shieldSprite, states);
}

void MainframeBoss::_checkShieldCollision() {
	for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player == nullptr || player->get<lif::Killable>()->isKilled()
				|| player->get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD))
		{
			continue;
		}
		if (lif::length(shieldSprite.getPosition() - player->getPosition()) <= SHIELD_DIAMETER / 2)
			player->dealDamage(SHIELD_DAMAGE);
	}
}

StateFunction MainframeBoss::_updateIdle() {
	if (clock->getElapsedTime() > IDLE_TIME) {
		// Choose attack
		static std::uniform_int_distribution<> dist(0, static_cast<int>(AttackType::N_ATTACKS) - 1);
		auto atkType = dist(lif::rng);
		clock->restart();
		switch (static_cast<AttackType>(atkType)) {
		case AttackType::ROTATING_SURGE:
			return BIND(_updateSurgeEntering);
		case AttackType::LIGHTNING_STORM:
			return BIND(_updateLightningEntering);
		case AttackType::SHIELD:
			return BIND(_updateShieldEntering);
		case AttackType::LASERS:
			return BIND(_updateLasersEntering);
		//case AttackType::SPAWN_ZAPS:
			//return BIND(_updateSpawnZapsEntering);
		default:
			break;
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
		_resetIdleAnim();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningEntering() {
	animated->setFrameTime("idle", sf::milliseconds(30));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(200, 255, 0));
	clock->restart();
	return BIND(_updateLightningWindup);
}

StateFunction MainframeBoss::_updateLightningWindup() {
	if (clock->getElapsedTime() > LIGHTNING_WINDUP_TIME) {
		clock->restart();
		nShots = 0;
		return BIND(_updateLightningShooting);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningShooting() {
	if (nShots == LIGHTNING_N_SHOTS) {
		clock->restart();
		_resetIdleAnim();
		return BIND(_updateLightningRecover);
	}
	if (clock->getElapsedTime() > LIGHTNING_SHOOT_DELAY) {
		const auto angle = lif::degrees(angleDist(lif::rng));
		const auto pos = lif::towards(position + (SIZE - sf::Vector2f(TILE_SIZE, TILE_SIZE)) * 0.5f,
				angle, 40);
		spawner->addSpawned(std::move(lif::BulletFactory::create(104, pos, angle, this)));
		++nShots;
		clock->restart();
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningRecover() {
	if (clock->getElapsedTime() > LIGHTNING_RECOVERY_TIME) {
		clock->restart();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateShieldEntering() {
	animated->setFrameTime("idle", sf::milliseconds(500));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(0, 80, 200));
	clock->restart();
	return BIND(_updateShieldWindup);
}

StateFunction MainframeBoss::_updateShieldWindup() {
	if (clock->getElapsedTime() > SHIELD_WINDUP_TIME) {
		clock->restart();
		vulnerable = false;
		return BIND(_updateShieldDamage);
	}
	
	auto col = shieldSprite.getFillColor();
	col.a = (clock->getElapsedTime() / SHIELD_WINDUP_TIME) * 180;
	shieldSprite.setFillColor(col);

	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateShieldDamage() {
	if (clock->getElapsedTime() > SHIELD_DAMAGE_TIME) {
		clock->restart();
		_resetIdleAnim();
		vulnerable = true;
		return BIND(_updateShieldRecover);
	}
	if (sparkClock->getElapsedTime() > sf::milliseconds(60)) {
		sparkClock->restart();
		auto pos = position + sf::Vector2f(SIZE.x - SHIELD_DIAMETER, SIZE.y - SHIELD_DIAMETER);
		static std::uniform_real_distribution<float> posDist(0, SHIELD_DIAMETER);
		pos.x += posDist(lif::rng);
		pos.y += posDist(lif::rng);
		auto spark = new lif::OneShotFX(pos, "spark.png", {
			sf::IntRect(0 * 2 * TILE_SIZE, 0, 2 * TILE_SIZE, 2 * TILE_SIZE),
			sf::IntRect(1 * 2 * TILE_SIZE, 0, 2 * TILE_SIZE, 2 * TILE_SIZE),
			sf::IntRect(2 * 2 * TILE_SIZE, 0, 2 * TILE_SIZE, 2 * TILE_SIZE),
			sf::IntRect(3 * 2 * TILE_SIZE, 0, 2 * TILE_SIZE, 2 * TILE_SIZE),
		});
		static std::uniform_real_distribution<float> scaleDist(0.2f, 1.2f);
		const auto scale = scaleDist(lif::rng);
		spark->get<lif::Animated>()->getSprite().setScale(scale, scale);
		spawner->addSpawned(spark);
	}

	auto col = shieldSprite.getFillColor();
	const auto a = lightSource->getColor().a * 180.f / 255;
	col.a = a;
	shieldSprite.setFillColor(col);

	_checkShieldCollision();

	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateShieldRecover() {
	if (clock->getElapsedTime() > SHIELD_RECOVERY_TIME) {
		clock->restart();
		return BIND(_updateIdle);
	}

	auto col = shieldSprite.getFillColor();
	col.a = (1 - clock->getElapsedTime() / SHIELD_RECOVERY_TIME) * 180;
	shieldSprite.setFillColor(col);

	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLasersEntering() {
	animated->setFrameTime("idle", sf::milliseconds(40));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(255, 40, 0));
	clock->restart();
	nShots = 0;
	const auto lifePerc = get<lif::Lifed>()->getRemainingLifePerc();
	lasersNShots = lif::lerp(LASERS_MIN_N_SHOTS, LASERS_MAX_N_SHOTS, 1 - lifePerc);
	lasersShootDelay = lif::lerp(LASERS_SHOOT_MIN_DELAY, LASERS_SHOOT_MAX_DELAY, lifePerc);
	return BIND(_updateLasersShooting);
}

StateFunction MainframeBoss::_updateLasersShooting() {
	if (nShots > lasersNShots) {
		clock->restart();
		_resetIdleAnim();
		return BIND(_updateLasersRecover);
	}
	if (clock->getElapsedTime() > lasersShootDelay) {
		clock->restart();
		const auto lvinfo = lm.getLevel()->getInfo();
		const auto nRows = lvinfo.width;
		const auto nCols = lvinfo.height;
		const auto orientation = static_cast<lif::TimedLaser::Orientation>(rand() % 2);
		int rowCol = 0;
		if (orientation == lif::TimedLaser::Orientation::HORIZONTAL) {
			std::uniform_int_distribution<> rowDist(0, nRows);
			rowCol = rowDist(lif::rng);
		} else {
			std::uniform_int_distribution<> colDist(0, nCols);
			rowCol = colDist(lif::rng);
		}
		spawner->addSpawned(new lif::TimedLaser(rowCol, orientation, LASERS_WARN_DURATION, LASERS_DAMAGE,
			{ lif::c_layers::PLAYERS }));
		++nShots;
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLasersRecover() {
	if (clock->getElapsedTime() > LASERS_RECOVERY_TIME) {
		clock->restart();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSpawnZapsEntering() {
	animated->setFrameTime("idle", sf::milliseconds(40));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(255, 0, 200));
	clock->restart();
	nShots = 0;
	return BIND(_updateSpawnZapsShooting);
}

StateFunction MainframeBoss::_updateSpawnZapsShooting() {
	if (nShots > N_SPAWNED_ZAPS) {
		clock->restart();
		_resetIdleAnim();
		return BIND(_updateSpawnZapsRecover);
	}
	if (clock->getElapsedTime() > ZAPS_SPAWN_DELAY) {
		clock->restart();
		const auto lvinfo = lm.getLevel()->getInfo();
		const auto nRows = lvinfo.width;
		const auto nCols = lvinfo.height;
		const auto orientation = static_cast<lif::TimedLaser::Orientation>(rand() % 2);
		int rowCol = 0;
		if (orientation == lif::TimedLaser::Orientation::HORIZONTAL) {
			std::uniform_int_distribution<> rowDist(0, nRows);
			rowCol = rowDist(lif::rng);
		} else {
			std::uniform_int_distribution<> colDist(0, nCols);
			rowCol = colDist(lif::rng);
		}
		spawner->addSpawned(new lif::TimedLaser(rowCol, orientation, LASERS_WARN_DURATION, LASERS_DAMAGE,
			{ lif::c_layers::PLAYERS }));
		++nShots;
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSpawnZapsRecover() {
	if (clock->getElapsedTime() > ZAPS_RECOVERY_TIME) {
		clock->restart();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}
