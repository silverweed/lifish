#include "Enemy.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "Scored.hpp"
#include "Lifed.hpp"
#include "LevelManager.hpp"
#include "Sounded.hpp"
#include "Collider.hpp"
#include "ZIndexed.hpp"
#include "utils.hpp"

using Game::Enemy;
using Game::TILE_SIZE;
using Game::Direction;

Enemy::Enemy(sf::Vector2f pos, unsigned short id, const Game::EnemyInfo& info)
	: Game::Entity(pos)
	, originalSpeed(info.speed)
{
	animated = addComponent(new Game::Animated(*this, 
		Game::getAsset(/*"graphics"*/ "test", std::string("enemy") + Game::to_string(id) + std::string(".png"))));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::Enemy));
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
	yellClock = addComponent(new Game::Clock(*this));
	dashClock = addComponent(new Game::Clock(*this));
	alienSprite = addComponent(new Game::AlienSprite(*this));
	addComponent(new Game::Scored(*this, id * 100));
	movingAnimator = addComponent(new Game::MovingAnimator(*this));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		animated->setAnimation("death");
		Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
	}));
	shooting = addComponent(new Game::Shooting(*this, info.attack));
	sighted = addComponent(new Game::Sighted(*this));

	drawProxy = std::unique_ptr<Game::EnemyDrawableProxy>(new Game::EnemyDrawableProxy(*this));
	addComponent(new Game::Drawable(*this, *drawProxy.get()));
	ai = addComponent(new Game::AI(*this, info.ai));

	// Ensure AI is updated _before_ moving
	auto it = std::find_if(components.begin(), components.end(), [this] (std::shared_ptr<Game::Component>& c) {
		return c.get() == ai;
	});
	std::rotate(components.begin(), it, components.end());

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

	shootFrame[moving->getDirection()].setPosition(position);
}

Game::Bullet* Enemy::checkShoot() const {
	if (killable->isKilled() || shooting->isRecharging() || morphed)
		return nullptr;
	
	const auto lm = sighted->getLevelManager();
	if (lm == nullptr)
		return nullptr;

	const auto& entitiesSeen = sighted->entitiesSeen(moving->getDirection());
	for (const auto& pair : entitiesSeen) {
		if (lm->isPlayer(pair.first)) {
			return shooting->shoot();
		}
	}

	return nullptr;
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

//////// EnemyDrawableProxy //////////

Game::EnemyDrawableProxy::EnemyDrawableProxy(Game::Enemy& e)
	: enemy(e)
{
	morphedAnim = e.get<Game::AlienSprite>()->get<Game::Animated>();
}

void Game::EnemyDrawableProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (enemy.isMorphed()) {
		target.draw(*morphedAnim, states);
	} else if (enemy.moving->isDashing() || enemy.shooting->isShooting()) {
		target.draw(enemy.shootFrame[enemy.moving->getDirection()], states);
	} else {
		target.draw(*enemy.animated, states);
	}
}

