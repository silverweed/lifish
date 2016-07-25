#include "Enemy.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "Scored.hpp"
#include "Lifed.hpp"
#include "LevelManager.hpp"
#include "Sounded.hpp"
#include "Collider.hpp"
#include "utils.hpp"

using Game::Enemy;
using Game::TILE_SIZE;

Enemy::Enemy(sf::Vector2f pos, unsigned short id, const Game::EnemyInfo& info)
	: Game::Entity(pos)
	, originalSpeed(info.speed)
{
	animated = addComponent(new Game::Animated(*this, 
		Game::getAsset(/*"graphics"*/ "test", std::string("enemy") + Game::to_string(id) + std::string(".png"))));
	addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, Game::Layers::ENEMIES));
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_death.ogg")),
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_yell.ogg")),
		// Note: *this is an invalid sound if enemy.attackType is not CONTACT. This is not an issue,
		// since in that case the sound never gets played, so the cache doesn't even load it.
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_attack.ogg"))
	}));
	addComponent(new Game::Lifed(*this, 1));
	moving = addComponent(new Game::AxisMoving(*this, BASE_SPEED * originalSpeed, Game::Direction::DOWN));
	attackClock = addComponent(new Game::Clock(*this));
	yellClock = addComponent(new Game::Clock(*this));
	dashClock = addComponent(new Game::Clock(*this));
	alienSprite = addComponent(new Game::AlienSprite(*this));
	addComponent(new Game::Scored(*this, id * 100));
	movingAnimator = addComponent(new Game::MovingAnimator(*this));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
	}));
	shooting = addComponent(new Game::Shooting(*this, info.attack));
	sighted = addComponent(new Game::Sighted(*this));

	drawProxy = std::unique_ptr<Game::EnemyDrawableProxy>(new Game::EnemyDrawableProxy(*this));
	addComponent(new Game::Drawable(*this, drawProxy.get()));
	ai = addComponent(new Game::AI(*this, info.ai));

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
	auto& a_shoot_down = animated->addAnimation("shoot_down");
	auto& a_shoot_up = animated->addAnimation("shoot_up");
	auto& a_shoot_right = animated->addAnimation("shoot_right");
	auto& a_shoot_left = animated->addAnimation("shoot_left");

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
	a_shoot_down.addFrame(sf::IntRect(
				0,
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	a_shoot_up.addFrame(sf::IntRect(
				TILE_SIZE, 
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	a_shoot_right.addFrame(sf::IntRect(
				2 * TILE_SIZE, 
				2 * TILE_SIZE, 
				TILE_SIZE, 
				TILE_SIZE));
	a_shoot_left.addFrame(sf::IntRect(
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
	// FIXME
	//moving->stop();
}

void Enemy::update() {
	Game::Entity::update();

	if (shooting->isShooting()) {
		const auto s = "shoot_" + Game::directionToString(moving->getDirection());
		if (!animated->isPlaying(s)) {
			animated->getSprite().setLooped(false);
			animated->setAnimation(s);
			animated->getSprite().play();
		}
		return;
	} else 
		animated->getSprite().setLooped(true);

	if (killable->isKilled() || shooting->isRecharging())
		return;
	
	//const auto lm = sighted->getLevelManager();
	//if (lm == nullptr)
		//return;

	//const auto& entitiesSeen = sighted->entitiesSeen(moving->getDirection());
	//for (const auto& pair : entitiesSeen) {
		//if (lm->isPlayer(pair.first)) {
			//shooting->shoot();
			//break;
		//}
	//}
}

void Enemy::setMorphed(bool b) {
	morphed = b;
}

void Enemy::_checkCollision(Game::Collider& coll) {
	if (coll.getLayer() != Game::Layers::EXPLOSIONS) return;
	auto lifed = get<Game::Lifed>();
	if (lifed->decLife(1) <= 0) {
		killable->kill();	
	}
}

/*void Enemy::draw(sf::RenderTarget& window) {
	if (dashing && !dead) {
		const unsigned short d = Game::directionToUshort(direction);
		shootFrame[d].setPosition(pos);
		window.draw(shootFrame[d]);
		return;
	} else if (shooting && !dead) {
		if (attackClock.getElapsedTime().asMilliseconds() < shootFrameTime) {
			const unsigned short d = Game::directionToUshort(direction);
			if (d < ANIM_DEATH) {
				shootFrame[d].setPosition(pos);
				window.draw(shootFrame[d]);
			}
			return;
		} else {
			shooting = false;
		}
	} else if (morphed) {
		alienSprite->draw(window, pos, dead ? Game::Direction::NONE : direction);
		return;
	}
	Game::MovingEntity::draw(window);
}

void Enemy::move(const Game::Direction dir) {
	if (blocked) {
		if (attackClock.getElapsedTime().asMilliseconds() < attack.blockTime) {
			animatedSprite.update(frameClock.restart());
			return;
		}
		blocked = false;
	}
	MovingEntity::move(dir);
}

bool Enemy::setDashing(bool b) {
	if (!b) {
		dashing = false;
		speed = originalSpeed;
		dashClock.restart();
	} else if (dashClock.getElapsedTime().asMilliseconds() >= 1000./attack.fireRate
			&& speed == originalSpeed) {
		dashing = true;
		speed *= 4;
		return true;
	}
	return false;
}

const std::string& Enemy::getSoundFile(unsigned short n) const {
	if (alienSprite != nullptr)
		return alienSprite->getSoundFile();
	return Game::Sounded::getSoundFile(n);
}

void Enemy::yell() {
	if (clocks->getElapsedTime("yell") >= YELL_DELAY) {
		Game::cache.playSound(getSoundFile(Game::Sounds::YELL));
		clocks->restart("yell");
	}
}*/

Game::EnemyDrawableProxy::EnemyDrawableProxy(Game::Enemy& e)
	: enemy(e)
{
	enemyAnim = e.get<Game::Animated>();
	morphedAnim = e.get<Game::AlienSprite>()->get<Game::Animated>();
}

void Game::EnemyDrawableProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (enemy.isMorphed())
		morphedAnim->draw(target, states);
	else
		enemyAnim->draw(target, states);
}
