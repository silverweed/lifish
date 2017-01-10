#include "Enemy.hpp"
#include "Foe.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "ai_functions.hpp"
#include "Letter.hpp"
#include "Shooting.hpp"
#include "Spawning.hpp"
#include "AutoShooting.hpp"
#include "RegularEntityDeath.hpp"
#include "Killable.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "MovingAnimator.hpp"
#include "AI.hpp"
#include "AlienSprite.hpp"
#include "AxisSighted.hpp"
#include "Lifed.hpp"
#include "LevelManager.hpp"
#include "Explosion.hpp"
#include "Sounded.hpp"
#include "collision_functions.hpp"
#include "Collider.hpp"
#include "ZIndexed.hpp"
#include "conf/enemy.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"
#include <sstream>

using lif::Enemy;
using lif::TILE_SIZE;
using lif::Direction;

Enemy::Enemy(const sf::Vector2f& pos, unsigned short id, const lif::EnemyInfo& info)
	: lif::Entity(pos)
	, id(id)
	, info(info)
	, originalSpeed(info.speed)
{
	addComponent(new lif::ZIndexed(*this, lif::conf::zindex::ENEMIES));
	addComponent(new lif::Sounded(*this, {
		std::make_pair("death", lif::getAsset("test", std::string("enemy")
					+ lif::to_string(id) + std::string("_death.ogg"))),
		std::make_pair("yell", lif::getAsset("test", std::string("enemy")
					+ lif::to_string(id) + std::string("_yell.ogg"))),
		// Note: this is an invalid sound if enemy.attackType is not CONTACT. This is not an issue,
		// since in that case the sound never gets played, so the cache doesn't even load it.
		std::make_pair("attack", lif::getAsset("test", std::string("enemy")
					+ lif::to_string(id) + std::string("_attack.ogg")))
	}));
	addComponent(new lif::Lifed(*this, 1, [this] (int newLife) {
		// on hurt
		if (newLife <= 0)
			killable->kill();	
	}));
	addComponent(new lif::Foe(*this));
	if (info.ai >= lif::ai_functions.size()) {
		std::stringstream ss;
		ss << "invalid AI number for Enemy: " << lif::to_string(info.ai) << "/" << lif::ai_functions.size();
		throw std::invalid_argument(ss.str());
	}
	ai = addComponent(new lif::AI(*this, lif::ai_functions[info.ai]));
	moving = addComponent(new lif::AxisMoving(*this, BASE_SPEED * originalSpeed, lif::Direction::DOWN));
	animated = addComponent(new lif::Animated(*this, 
		lif::getAsset("graphics", std::string("enemy") + lif::to_string(id) + std::string(".png"))));
	yellClock = addComponent(new lif::Clock(*this));
	dashClock = addComponent(new lif::Clock(*this));
	alienSprite = addComponent(new lif::AlienSprite(*this));
	addComponent(new lif::Scored(*this, id * 100));
	movingAnimator = addComponent(new lif::MovingAnimator(*this));
	killable = addComponent(new lif::Killable(*this, [this] () {
		// on kill
		death->kill(); 
	}, [this] () {
		return death->isKillInProgress(); 
	}));
	// Spawn letter on death
	addComponent(new lif::Spawning(*this, [this] (const lif::Spawning& spw) {
		return morphed && spw.nSpawned() == 0 && killable->isKilled() && !killable->isKillInProgress();
	}, [this] () {
		return new lif::Letter(position, lif::Letter::randomId());
	}));
	death = addComponent(new lif::RegularEntityDeath(*this, lif::conf::enemy::DEATH_TIME));
	shooting = addComponent(new lif::Shooting(*this, info.attack));
	autoShooting = addComponent(new lif::AutoShooting(*this));
	sighted = addComponent(new lif::AxisSighted(*this));

	drawProxy = std::unique_ptr<lif::EnemyDrawableProxy>(new lif::EnemyDrawableProxy(*this));
	addComponent(new lif::Drawable(*this, *drawProxy));

	auto hurt_by_explosion = lif::hurtByExplosions(*this, lif::CFO_TAKE_SINGLE_HIT);
	collider = addComponent(new lif::Collider(*this, [this, hurt_by_explosion] (lif::Collider& coll) {
		// on collision
		if (!_checkCollision(coll))
			hurt_by_explosion(coll);
	}, lif::c_layers::ENEMIES));

	unsigned short death_n_frames = 2;
	switch (id) {
	case 3:
		death_n_frames = 4;
		break;
	default:
		break;
	}

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

	animated->addAnimation("idle", { sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE) });

	for (auto& frame : shootFrame) {
		frame.setTexture(*a_down.getSpriteSheet());
		frame.setOrigin(origin);
	}

	shootFrame[Direction::DOWN].setTextureRect(sf::IntRect(
				0,
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	shootFrame[Direction::UP].setTextureRect(sf::IntRect(
				TILE_SIZE, 
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	shootFrame[Direction::RIGHT].setTextureRect(sf::IntRect(
				2 * TILE_SIZE, 
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	shootFrame[Direction::LEFT].setTextureRect(sf::IntRect(
				3 * TILE_SIZE, 
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));

	auto& a_death = animated->addAnimation("death");
	for (unsigned i = 0; i < death_n_frames; ++i) 
		a_death.addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_down);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();
}

void Enemy::update() {
	lif::Entity::update();

	if (moving->getDirection() != lif::Direction::NONE) {
		shootFrame[moving->getDirection()].setPosition(position);
		_checkShoot();
	}
}

void Enemy::_checkShoot() {
	if (killable->isKilled() || shooting->isRecharging() || morphed 
			|| (shooting->getAttack().type & lif::AttackType::CONTACT))
		return;
	
	const auto& entitiesSeen = sighted->entitiesSeen(moving->getDirection());
	for (const auto& pair : entitiesSeen) {
		const auto entity = pair.first.lock();
		if (_inRange(entity.get()) && std::dynamic_pointer_cast<const lif::Player>(entity) != nullptr) {
			autoShooting->shoot();
			return;
		}
	}
}

bool Enemy::_checkCollision(lif::Collider& coll) {
	if (coll.getLayer() == lif::c_layers::PLAYERS 
			&& (shooting->getAttack().type & lif::AttackType::CONTACT)
			&& !shooting->isRecharging())
	{
		shooting->shoot();
		return true;
	}
	return false;
}

bool Enemy::_inRange(const lif::Entity *const e) const {
	const auto atk = shooting->getAttack();
	return e == nullptr || atk.bullet.range < 0
		|| lif::manhattanDistance(e->getPosition(), position) <= atk.bullet.range;
}

void Enemy::setMorphed(bool b) {
	morphed = b;
}

//////// EnemyDrawableProxy //////////

lif::EnemyDrawableProxy::EnemyDrawableProxy(const lif::Enemy& e)
	: enemy(e)
	, morphedAnim(*e.get<lif::AlienSprite>()->get<lif::Animated>())
{}

void lif::EnemyDrawableProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (enemy.isMorphed()) {
		target.draw(morphedAnim, states);
	} else if (!enemy.killable->isKilled() && (enemy.moving->isDashing() || enemy.shooting->isShooting())) {
		target.draw(enemy.shootFrame[enemy.moving->getDirection()], states);
	} else {
		target.draw(*enemy.animated, states);
	}
}
