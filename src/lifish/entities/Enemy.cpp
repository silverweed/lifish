#include "Enemy.hpp"
#include "AI.hpp"
#include "AlienSprite.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "AxisSighted.hpp"
#include "Bonusable.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Explosion.hpp"
#include "Foe.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Letter.hpp"
#include "LevelManager.hpp"
#include "Lifed.hpp"
#include "MovingAnimator.hpp"
#include "Player.hpp"
#include "RegularEntityDeath.hpp"
#include "Scored.hpp"
#include "Shooting.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Time.hpp"
#include "Warpable.hpp"
#include "ZIndexed.hpp"
#include "ai_functions.hpp"
#include "collision_functions.hpp"
#include "conf/enemy.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"
#include <random>
#include <sstream>

using lif::Enemy;
using lif::TILE_SIZE;
using lif::Direction;
using namespace std::literals::string_literals;

Enemy::Enemy(const sf::Vector2f& pos, unsigned short id, const lif::EnemyInfo& info)
	: lif::Entity(pos)
	, id(id)
	, info(info)
	, originalSpeed(info.speed)
{
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::ENEMIES);
	sounded = addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "enemy"s + lif::to_string(id) + "_death.ogg"s),
		lif::sid("yell"), lif::getAsset("sounds", "enemy"s + lif::to_string(id) + "_yell.ogg"s),
		// Note: this is an invalid sound if enemy.attackType is not CONTACT. This is not an issue,
		// since in that case the sound never gets played, so the cache doesn't even load it.
		lif::sid("attack"), lif::getAsset("sounds", "enemy"s + lif::to_string(id) + "_attack.ogg"s)
	);
	addComponent<lif::Lifed>(*this, lif::conf::enemy::BASE_LIFE, [this] (int damage, int newLife) {
		// on hurt
		if (newLife <= 0)
			killable->kill();
		else if (damage > 0)
			bonusable->giveBonus(lif::BonusType::SHIELD, lif::conf::enemy::DAMAGE_SHIELD_TIME);
	});
	addComponent<lif::Foe>(*this);
	addComponent<lif::Warpable>(*this);
	bonusable = addComponent<lif::Bonusable>(*this);
	if (info.ai >= lif::ai_functions.size()) {
		std::stringstream ss;
		ss << "invalid AI number for Enemy: " << lif::to_string(info.ai+1) << "/" << lif::ai_functions.size();
		throw std::invalid_argument(ss.str());
	}
	ai = addComponent<lif::AI>(*this, lif::ai_functions[info.ai]);
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::enemy::BASE_SPEED * originalSpeed, lif::Direction::DOWN);
	animated = addComponent<lif::Animated>(*this,
			lif::getAsset("graphics", "enemy") + lif::to_string(id) + ".png"s);
	alienSprite = addComponent<lif::AlienSprite>(*this);
	addComponent<lif::Scored>(*this, id * 100);
	movingAnimator = addComponent<lif::MovingAnimator>(*this);
	killable = addComponent<lif::Killable>(*this, [this] () {
		// on kill
		animated->getSprite().setLooped(true);
		death->kill();
	}, [this] () {
		return death->isKillInProgress();
	});
	// Spawn letter on death
	addComponent<lif::Spawning>(*this, [this] (const lif::Spawning& spw) {
		return morphed && spw.nSpawned() == 0 && killable->isKilled() && !killable->isKillInProgress();
	}, [this] () {
		return new lif::Letter(position, lif::Letter::randomId());
	});
	death = addComponent<lif::RegularEntityDeath>(*this, lif::conf::enemy::DEATH_TIME);
	shooting = addComponent<lif::Shooting>(*this, info.attack);
	sighted = addComponent<lif::AxisSighted>(*this);

	drawProxy = std::make_unique<lif::EnemyDrawableProxy>(*this);
	// Keep this AFTER drawProxy!!!
	addComponent<lif::Drawable>(*this, *drawProxy);

	auto hurt_by_explosion = lif::hurtByExplosions(*this);
	collider = addComponent<lif::Collider>(*this, [this, hurt_by_explosion] (lif::Collider& coll) {
		// on collision
		if (!_checkCollision(coll))
			hurt_by_explosion(coll);
	}, lif::c_layers::ENEMIES);

	unsigned short death_n_frames = 2;
	switch (id) {
	case 3:
		death_n_frames = 4;
		break;
	default:
		break;
	}

	animated->setDefaultFrameTime(sf::seconds(0.12));
	auto& a_down = animated->addAnimation("walk_down");
	auto& a_up = animated->addAnimation("walk_up");
	auto& a_right = animated->addAnimation("walk_right");
	auto& a_left = animated->addAnimation("walk_left");

	for (unsigned i = 0; i < WALK_N_FRAMES; ++i) {
		a_down.addFrame(sf::IntRect(
					i * TILE_SIZE,
					0,
					TILE_SIZE,
					TILE_SIZE));
		a_up.addFrame(sf::IntRect(
					(WALK_N_FRAMES + i) * TILE_SIZE,
					0,
					TILE_SIZE,
					TILE_SIZE));
		a_right.addFrame(sf::IntRect(
					i * TILE_SIZE,
					TILE_SIZE,
					TILE_SIZE,
					TILE_SIZE));
		a_left.addFrame(sf::IntRect(
					(WALK_N_FRAMES + i) * TILE_SIZE,
					TILE_SIZE,
					TILE_SIZE,
					TILE_SIZE));
	}

	animated->addAnimation("idle_down", { sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_up", { sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_right", { sf::IntRect(0, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_left", { sf::IntRect(0, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	const auto shootFrameTime = (info.attack.type & lif::AttackType::CONTACT) ? 0.1 : 0.3;
	animated->setFrameTime("shoot_down", sf::seconds(shootFrameTime));
	animated->addAnimation("shoot_down", { sf::IntRect(0, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	animated->setFrameTime("shoot_up", sf::seconds(shootFrameTime));
	animated->addAnimation("shoot_up", { sf::IntRect(TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	animated->setFrameTime("shoot_right", sf::seconds(shootFrameTime));
	animated->addAnimation("shoot_right", { sf::IntRect(2 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });
	animated->setFrameTime("shoot_left", sf::seconds(shootFrameTime));
	animated->addAnimation("shoot_left", { sf::IntRect(3 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE) });

	auto& a_death = animated->addAnimation("death");
	for (unsigned i = 0; i < death_n_frames; ++i)
		a_death.addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_down);
	animatedSprite.setLooped(true);
	animatedSprite.pause();

	if (info.ai <= 1) {
		canYell = true;
		nextYellTime = getNextYellTime();
	}
}

void Enemy::update() {
	lif::Entity::update();

	yellT += lif::time.getDelta();

	if (killable->isKilled())
		return;

	if (moving->getDirection() != lif::Direction::NONE)
		_checkShoot();

	if (!shooting->isShooting() && shootingAnim && !animated->getSprite().isPlaying()) {
		animated->setAnimation(lif::sid("walk_" + lif::directionToStringOr(moving->getDirection(), "down")));
		animated->getSprite().setLooped(true);
		animated->getSprite().play();
		shootingAnim = false;
	}

	if (canYell && yellT >= nextYellTime) {
		yellT = sf::Time::Zero;
		nextYellTime = getNextYellTime();
		if (!morphed)
			lif::cache.playSound(sounded->getSoundFile("yell"));
	}
}

void Enemy::_checkShoot() {
	if (killable->isKilled() || shooting->isRecharging() || morphed
			|| (shooting->getAttack().type & lif::AttackType::CONTACT))
		return;

	const auto& entitiesSeen = sighted->entitiesSeen(moving->getDirection());
	for (const auto& pair : entitiesSeen) {
		const auto entity = pair.first;
		if (_inRange(entity) && ai->getLevelManager()->isPlayer(*entity)) {
			if (!entity->get<lif::Killable>()->isKilled()) {
				_setShootAnim();
				shooting->shoot(entity->getPosition());
				if (info.ai > 2)
					lif::cache.playSound(sounded->getSoundFile("yell"));
				return;
			}
		}
	}
}

void Enemy::_setShootAnim() {
	if (!shootingAnim) {
		if (moving->getDirection() == lif::Direction::NONE)
			return;
		animated->setAnimation(lif::sid(
				"shoot_" + lif::directionToString(moving->getDirection())));
		animated->getSprite().play();
		animated->getSprite().setLooped(false, false);
		shootingAnim = true;
	}
}

bool Enemy::_checkCollision(lif::Collider& coll) {
	if (killable->isKilled() || morphed)
		return false;

	if (coll.getLayer() == lif::c_layers::PLAYERS
			&& (shooting->getAttack().type & lif::AttackType::CONTACT)
			&& !shooting->isRecharging()
			&& !coll.getOwner().get<lif::Killable>()->isKilled()
			&& lif::nOverlappedPixels(coll.getRect(), collider->getRect()) > 2)
	{
		_setShootAnim();
		shooting->shoot();
		return true;
	}
	return false;
}

bool Enemy::_inRange(const lif::Entity *const e) const {
	const auto atk = shooting->getAttack();
	return e == nullptr || atk.range < 0
		|| lif::manhattanDistance(e->getPosition(), position) <= atk.range;
}

void Enemy::setMorphed(bool b) {
	morphed = b;
	if (b) {
		sounded->setSoundFile("death", lif::getAsset("sounds", "alien_death.ogg"));
	} else {
		sounded->setSoundFile("death",
			lif::getAsset("sounds", "enemy"s + lif::to_string(id) + "_death.ogg"s));
	}
}

sf::Time Enemy::getNextYellTime() const {
	using namespace lif::conf::enemy;
	static std::uniform_real_distribution<float> d(YELL_INTERVAL_MIN.asSeconds(), YELL_INTERVAL_MAX.asSeconds());
	return sf::seconds(d(lif::rng));
}

//////// EnemyDrawableProxy //////////

lif::EnemyDrawableProxy::EnemyDrawableProxy(const lif::Enemy& e)
	: enemy(e)
	, morphedAnim(*e.get<lif::AlienSprite>()->get<lif::Animated>())
{}

void lif::EnemyDrawableProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	const auto& anim = enemy.isMorphed() ? morphedAnim : *enemy.animated;
	target.draw(anim, states);

	if (enemy.bonusable->hasBonus(lif::BonusType::SHIELD)) {
		const float s = enemy.bonusable->getElapsedTime(lif::BonusType::SHIELD).asSeconds();
		const float diff = s - std::floor(s);
		if (enemy.bonusable->getRemainingTime(lif::BonusType::SHIELD) > sf::seconds(3)
				|| 4 * diff - std::floor(4 * diff) < 0.5)
		{
			AnimatedSprite shieldSprite(anim.getSprite());
			// TODO: scale & offset
			shieldSprite.setColor(sf::Color(200, 0, 200, 180));
			target.draw(shieldSprite, states);
		}
	}
}
