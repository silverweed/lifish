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
#include "Collider.hpp"
#include "ZIndexed.hpp"
#include "enemy.hpp"
#include "zindex.hpp"
#include "utils.hpp"
#include <sstream>

using Game::Enemy;
using Game::TILE_SIZE;
using Game::Direction;

Enemy::Enemy(sf::Vector2f pos, unsigned short id, const Game::EnemyInfo& info)
	: Game::Entity(pos)
	, id(id)
	, info(info)
	, originalSpeed(info.speed)
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::ENEMIES));
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_death.ogg")),
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_yell.ogg")),
		// Note: this is an invalid sound if enemy.attackType is not CONTACT. This is not an issue,
		// since in that case the sound never gets played, so the cache doesn't even load it.
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_attack.ogg"))
	}));
	addComponent(new Game::Lifed(*this, 1, [this] (int newLife) {
		// on hurt
		if (newLife <= 0)
			killable->kill();	
	}));
	addComponent(new Game::Foe(*this));
	if (info.ai >= Game::ai_functions.size()) {
		std::stringstream ss;
		ss << "invalid AI number for Enemy: " << Game::to_string(info.ai) << "/" << Game::ai_functions.size();
		throw std::invalid_argument(ss.str());
	}
	ai = addComponent(new Game::AI(*this, Game::ai_functions[info.ai]));
	moving = addComponent(new Game::AxisMoving(*this, BASE_SPEED * originalSpeed, Game::Direction::DOWN));
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, Game::Layers::ENEMIES));
	animated = addComponent(new Game::Animated(*this, 
		Game::getAsset("graphics", std::string("enemy") + Game::to_string(id) + std::string(".png"))));
	yellClock = addComponent(new Game::Clock(*this));
	dashClock = addComponent(new Game::Clock(*this));
	alienSprite = addComponent(new Game::AlienSprite(*this));
	addComponent(new Game::Scored(*this, id * 100));
	movingAnimator = addComponent(new Game::MovingAnimator(*this));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		death->kill(); 
	}, [this] () {
		return death->isKillInProgress(); 
	}));
	// Spawn letter on death
	addComponent(new Game::Spawning(*this, [this] (const Game::Spawning& spw) {
		return morphed && spw.nSpawned() == 0 && killable->isKilled() && !killable->isKillInProgress();
	}, [this] () {
		return new Game::Letter(position, Game::Letter::randomId());
	}));
	death = addComponent(new Game::RegularEntityDeath(*this, Game::Conf::Enemy::DEATH_TIME));
	shooting = addComponent(new Game::Shooting(*this, info.attack));
	autoShooting = addComponent(new Game::AutoShooting(*this));
	sighted = addComponent(new Game::AxisSighted(*this));

	drawProxy = std::unique_ptr<Game::EnemyDrawableProxy>(new Game::EnemyDrawableProxy(*this));
	addComponent(new Game::Drawable(*this, *drawProxy));

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

	for (unsigned short i = 0; i < WALK_N_FRAMES; ++i) {
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
	for (unsigned short i = 0; i < death_n_frames; ++i) 
		a_death.addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_down);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();
}

void Enemy::update() {
	Game::Entity::update();

	if (moving->getDirection() != Game::Direction::NONE) {
		shootFrame[moving->getDirection()].setPosition(position);
		_checkShoot();
	}
}

void Enemy::_checkShoot() {
	if (killable->isKilled() || shooting->isRecharging() || morphed 
			|| (shooting->getAttack().type & Game::AttackType::CONTACT))
		return;
	
	const auto& entitiesSeen = sighted->entitiesSeen(moving->getDirection());
	for (const auto& pair : entitiesSeen) {
		const auto entity = pair.first.lock();
		if (_inRange(entity.get()) && std::dynamic_pointer_cast<const Game::Player>(entity) != nullptr) {
			autoShooting->shoot();
			return;
		}
	}
}

bool Enemy::_inRange(const Game::Entity *const e) const {
	const auto atk = autoShooting->getShooting()->getAttack();
	return e == nullptr || atk.range < 0 || Game::manhattanDistance(e->getPosition(), position) <= atk.range;
}

void Enemy::setMorphed(bool b) {
	morphed = b;
}

void Enemy::_checkCollision(Game::Collider& coll) {
	if (coll.getLayer() == Game::Layers::PLAYERS && (shooting->getAttack().type & Game::AttackType::CONTACT)) {
		if (!shooting->isRecharging())
			shooting->shoot();
		return;
	}
	if (coll.getLayer() != Game::Layers::EXPLOSIONS) return;
	auto lifed = get<Game::Lifed>();
	const auto& expl = static_cast<const Game::Explosion&>(coll.getOwner());
	if (lifed->decLife(expl.getDamage()) <= 0) {
		const auto source = dynamic_cast<const Game::Player*>(expl.getSourceEntity());
		if (source != nullptr)
			get<Game::Scored>()->setTarget(source->getInfo().id);
	}
}

//////// EnemyDrawableProxy //////////

Game::EnemyDrawableProxy::EnemyDrawableProxy(const Game::Enemy& e)
	: enemy(e)
	, morphedAnim(*e.get<Game::AlienSprite>()->get<Game::Animated>())
{}

void Game::EnemyDrawableProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (enemy.isMorphed()) {
		target.draw(morphedAnim, states);
	} else if (!enemy.killable->isKilled() && (enemy.moving->isDashing() || enemy.shooting->isShooting())) {
		target.draw(enemy.shootFrame[enemy.moving->getDirection()], states);
	} else {
		target.draw(*enemy.animated, states);
	}
}
