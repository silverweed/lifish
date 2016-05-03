#include "Enemy.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "Scored.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"

using Game::Enemy;
using Game::TILE_SIZE;

Enemy::Enemy(sf::Vector2f pos, unsigned short id, float speed, const Game::Attack& attack)
	: Game::Entity(pos)
	, originalSpeed(speed)
	, attackAlign(-1, -1)
{
	animated = addComponent(new Game::Animated(this, 
		Game::getAsset("graphics", std::string("enemy") + Game::to_string(id) + std::string(".png"))));
	addComponent(new Game::Drawable(this, animated));
	addComponent(new Game::Sounded(this, {
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_death.ogg")),
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_yell.ogg")),
		// Note: this is an invalid sound if enemy.attackType is not CONTACT. This is not an issue,
		// since in that case the sound never gets played, so the cache doesn't even load it.
		Game::getAsset("test", std::string("enemy") + Game::to_string(id) + std::string("_attack.ogg"))
	}));
	addComponent(new Game::Lifed(this, 1));
	addComponent(new Game::AxisMoving(this, speed, Game::Direction::DOWN));
	clocks = addComponent(new Game::Clock<3>(this, { "attack", "yell", "dash" }));
	alienSprite = addComponent(new Game::AlienSprite(this));
	addComponent(new Game::Scored(this, id * 100));
	addComponent(new Game::Shooting(this, attack));
	movingAnimator = addComponent(new Game::MovingAnimator(this));

	unsigned short death_n_frames = 2;
	switch (id) {
	case 3:
		death_n_frames = 4;
		break;
	default:
		break;
	}

	auto& a_down = animated->addAnimation("down");
	auto& a_up = animated->addAnimation("up");
	auto& a_right = animated->addAnimation("right");
	auto& a_left = animated->addAnimation("left");
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
}

void Enemy::update() {
	Game::Entity::update();

	if (shooting->isShooting()) {
		animated->setAnimation("shoot_" + Game::directionToString(direction));
		animated->play();
		return;
	}
	
	movingAnimator->update();
}

void setMorphed(bool b) {
	morphed = b;
	auto animated = get<Game::Animated>();
	if (morphed) {
	} else {

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
}*/

void Enemy::setMorphed(bool b) {
	morphed = b;
	if (morphed) {
		alienSprite = new Game::AlienSprite;
		alienSprite->setOrigin(animatedSprite.getOrigin());
	} else if (alienSprite != nullptr) {
		delete alienSprite;
		alienSprite = nullptr;
	}
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
	if (yellClock.getElapsedTime().asMilliseconds() >= YELL_DELAY) {
		Game::cache.playSound(getSoundFile(Game::Sounds::YELL));
		yellClock.restart();
	}
}
