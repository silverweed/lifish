#include "MainframeBoss.hpp"
#include "Animated.hpp"
#include "Bonusable.hpp"
#include "BufferedSpawner.hpp"
#include "Bullet.hpp"
#include "BulletFactory.hpp"
#include "Drawable.hpp"
#include "Enemy.hpp"
#include "Fixed.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "Level.hpp"
#include "Lifed.hpp"
#include "LightSource.hpp"
#include "OneShotFX.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Surge.hpp"
#include "SurgeWarn.hpp"
#include "Time.hpp"
#include "TimedLaser.hpp"
#include "conf/boss.hpp"
#include "game.hpp"
#include "spawn_functions.hpp"
#include <cassert>
#include <cstdlib>
#include <random>
#ifndef RELEASE
	#include "DebugPainter.hpp"
#endif

#define BIND(f) std::bind(&MainframeBoss:: f, this)

using lif::MainframeBoss;
using lif::TILE_SIZE;
using StateFunction = lif::ai::StateFunction;
using namespace lif::conf::boss::mainframe_boss;

static const sf::Vector2f SIZE(5 * TILE_SIZE, 5 * TILE_SIZE);
static const sf::Time IDLE_FRAME_TIME = sf::milliseconds(100);
static std::uniform_real_distribution<float> angleDist(0, 360);
constexpr auto SHIELD_DIAMETER = 8 * TILE_SIZE;

MainframeBoss::MainframeBoss(const sf::Vector2f& pos, lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
	, shieldSprite(SHIELD_DIAMETER / 2)
{
	addComponent<lif::Fixed>(*this);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "mainframe_boss.png"));
	animated->setFrameTime("idle", IDLE_FRAME_TIME);
	animated->addAnimation("idle", {
		sf::IntRect(0 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 0, SIZE.x, SIZE.y),
		sf::IntRect(4 * SIZE.x, 0, SIZE.x, SIZE.y),
	}, true);
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "mainframe_death.ogg"),
		lif::sid("hurt"), lif::getAsset("sounds", "mainframe_hurt.ogg")
	);
	addComponent<lif::Lifed>(*this, LIFE);
	addComponent<lif::Scored>(*this, VALUE);
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
	lif::Boss::update();

	const auto delta = lif::time.getDelta();
	sparkT += delta;
	atkT += delta;

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
	if (atkT > IDLE_TIME) {
		/// Choose attack
		const auto nEnemies = lm.getEntities().size<lif::Enemy>();
		std::uniform_int_distribution<> dist(0, static_cast<int>(AttackType::N_ATTACKS) - 1
				- (nEnemies > 2) // disable SPAWN_ZAPS if there are too many enemies
		);
		auto atkType = dist(lif::rng);
		atkT = sf::Time::Zero;
		switch (static_cast<AttackType>(atkType)) {
		case AttackType::ROTATING_SURGE:
			return BIND(_updateSurgeEntering);
		case AttackType::LIGHTNING_STORM:
			return BIND(_updateLightningEntering);
		case AttackType::SHIELD:
			return BIND(_updateShieldEntering);
		case AttackType::LASERS:
			return BIND(_updateLasersEntering);
		case AttackType::SPAWN_ZAPS:
			return BIND(_updateSpawnZapsEntering);
		default:
			assert(0 && "[MainframeBoss] Chosen invalid attack type!");
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
	atkT = sf::Time::Zero;
	return BIND(_updateSurgeWindup);
}

StateFunction MainframeBoss::_updateSurgeWindup() {
	if (atkT > SURGE_WINDUP_TIME) {
		spawner->addSpawned(new lif::Surge(position + SIZE * 0.5f, lm, nextAttackAngle,
				SURGE_ROT_PER_SECOND, SURGE_SPANNED_ANGLE));
		atkT = sf::Time::Zero;
		return BIND(_updateSurgeRecover);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSurgeRecover() {
	if (atkT > SURGE_RECOVER_TIME) {
		atkT = sf::Time::Zero;
		_resetIdleAnim();
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningEntering() {
	animated->setFrameTime("idle", sf::milliseconds(30));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(200, 255, 0));
	atkT = sf::Time::Zero;
	return BIND(_updateLightningWindup);
}

StateFunction MainframeBoss::_updateLightningWindup() {
	if (atkT > LIGHTNING_WINDUP_TIME) {
		atkT = sf::Time::Zero;
		nShots = 0;
		return BIND(_updateLightningShooting);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningShooting() {
	if (nShots == LIGHTNING_N_SHOTS) {
		atkT = sf::Time::Zero;
		_resetIdleAnim();
		return BIND(_updateLightningRecover);
	}
	if (atkT > LIGHTNING_SHOOT_DELAY) {
		const auto angle = lif::degrees(angleDist(lif::rng));
		const auto pos = lif::towards(position + (SIZE - sf::Vector2f(TILE_SIZE, TILE_SIZE)) * 0.5f,
				angle, 40);
		spawner->addSpawned(lif::BulletFactory::create(104, pos, angle, this));
		++nShots;
		atkT = sf::Time::Zero;
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLightningRecover() {
	if (atkT > LIGHTNING_RECOVERY_TIME) {
		atkT = sf::Time::Zero;
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateShieldEntering() {
	animated->setFrameTime("idle", sf::milliseconds(500));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(0, 80, 200));
	atkT = sf::Time::Zero;
	return BIND(_updateShieldWindup);
}

StateFunction MainframeBoss::_updateShieldWindup() {
	if (atkT > SHIELD_WINDUP_TIME) {
		atkT = sf::Time::Zero;
		vulnerable = false;
		return BIND(_updateShieldDamage);
	}

	auto col = shieldSprite.getFillColor();
	col.a = (atkT / SHIELD_WINDUP_TIME) * 180;
	shieldSprite.setFillColor(col);

	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateShieldDamage() {
	if (atkT > SHIELD_DAMAGE_TIME) {
		atkT = sf::Time::Zero;
		_resetIdleAnim();
		vulnerable = true;
		return BIND(_updateShieldRecover);
	}
	if (sparkT > sf::milliseconds(60)) {
		sparkT = sf::Time::Zero;
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
	if (atkT > SHIELD_RECOVERY_TIME) {
		atkT = sf::Time::Zero;
		return BIND(_updateIdle);
	}

	auto col = shieldSprite.getFillColor();
	col.a = (1 - atkT / SHIELD_RECOVERY_TIME) * 180;
	shieldSprite.setFillColor(col);

	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateLasersEntering() {
	animated->setFrameTime("idle", sf::milliseconds(40));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(255, 40, 0));
	atkT = sf::Time::Zero;
	nShots = 0;
	const auto lifePerc = get<lif::Lifed>()->getRemainingLifePerc();
	lasersNShots = lif::lerp(LASERS_MIN_N_SHOTS, LASERS_MAX_N_SHOTS, 1 - lifePerc);
	lasersShootDelay = lif::lerp(LASERS_SHOOT_MIN_DELAY, LASERS_SHOOT_MAX_DELAY, lifePerc);
	return BIND(_updateLasersShooting);
}

StateFunction MainframeBoss::_updateLasersShooting() {
	if (nShots > lasersNShots) {
		atkT = sf::Time::Zero;
		_resetIdleAnim();
		return BIND(_updateLasersRecover);
	}
	if (atkT > lasersShootDelay) {
		atkT = sf::Time::Zero;
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
	if (atkT > LASERS_RECOVERY_TIME) {
		atkT = sf::Time::Zero;
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSpawnZapsEntering() {
	animated->setFrameTime("idle", sf::milliseconds(40));
	animated->setAnimation("idle");
	animated->getSprite().setColor(sf::Color(255, 0, 200));
	atkT = sf::Time::Zero;
	nShots = 0;
	return BIND(_updateSpawnZapsShooting);
}

StateFunction MainframeBoss::_updateSpawnZapsShooting() {
	if (nShots > N_SPAWNED_ENEMIES) {
		atkT = sf::Time::Zero;
		_resetIdleAnim();
		return BIND(_updateSpawnZapsRecover);
	}
	if (atkT > ZAPS_SPAWN_DELAY) {
		atkT = sf::Time::Zero;

		auto spawner = get<lif::BufferedSpawner>();
		// TODO: add cool spawn effect
		lif::spawnInFreeTiles(spawner, lm, SPAWNED_ENEMY_ID, N_SPAWNED_ENEMIES, 4);
		nShots = N_SPAWNED_ENEMIES + 1;
	}
	return std::move(stateFunction);
}

StateFunction MainframeBoss::_updateSpawnZapsRecover() {
	if (atkT > ZAPS_RECOVERY_TIME) {
		atkT = sf::Time::Zero;
		return BIND(_updateIdle);
	}
	return std::move(stateFunction);
}
