#include "Boss.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "Clock.hpp"
#include "BossExplosion.hpp"
#include "Spawning.hpp"
#include "Collider.hpp"
#include "Killable.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "Explosion.hpp"
#include "Lifed.hpp"
#include "zindex.hpp"
#include "boss.hpp"
#include "Foe.hpp"
#include <cassert>

using Game::Boss;
using Game::TILE_SIZE;

Boss::Boss(const sf::Vector2f& pos)
	: Game::Entity(pos)
	, drawProxy(*this)
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::BOSSES));
	addComponent(new Game::Foe(*this));
	explClock = addComponent(new Game::Clock(*this));
	hurtClock = addComponent(new Game::Clock(*this));
	hurtClock->add(Game::Conf::Boss::HURT_TIME);
	deathClock = addComponent(new Game::Clock(*this));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		// kill in progress
		return _killInProgress();
	}));
	addComponent(new Game::Spawning(*this, [this] (const Game::Spawning&) {
		return killable && killable->isKilled() 
			&& explClock->getElapsedTime() >= sf::milliseconds(100);
	}, [this] () {
		explClock->restart();
		// Calculate a random location inside the boss
		const auto bpos = collider->getPosition();
		std::uniform_real_distribution<float> distX(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().x/TILE_SIZE - 0.5)),
		                                      distY(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().y/TILE_SIZE - 0.5));
		const float x = distX(rng),
		            y = distY(rng);
		auto expl = new Game::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y));
		Game::cache.playSound(expl->get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
		return expl;
	}));
	addComponent(new Game::Drawable(*this, drawProxy));
}

Game::Entity* Boss::init() {
	Game::Entity::init();
	if (collider == nullptr)
		throw std::logic_error("Collider is null for " + toString() + "!");
	if (animated == nullptr)
		throw std::logic_error("Animated is null for " + toString() + "!");
	if (killable == nullptr)
		throw std::logic_error("Killable is null for " + toString() + "!");
	return this;
}

void Boss::_hurt() {
	hurtClock->restart();
}

void Boss::_kill() {
	deathClock->restart();
	collider->setLayer(Game::Layers::DEFAULT);
}

bool Boss::_killInProgress() const {
	return deathClock->getElapsedTime() < Game::Conf::Boss::DEATH_TIME;
}

void Boss::_checkCollision(Game::Collider& coll) {
	if (coll.getLayer() != Game::Layers::EXPLOSIONS) return;

	auto& expl = static_cast<Game::Explosion&>(coll.getOwnerRW());
	if (expl.hasDamaged(this)) return;

	// Calculate how many explosion tiles overlap with boss's ones
	const auto brect = collider->getRect();
	const auto crect = coll.getRect();
	const short x = std::max(brect.left, crect.left),
	            wx = std::min(brect.left + brect.width, crect.left + crect.width),
	            y = std::max(brect.top, crect.top),
	            wy = std::min(brect.top + brect.height, crect.top + crect.height);
	
	assert(x > 0 && wx > 0 && y > 0 && wy > 0);

	const unsigned damage = std::round(float(wx - x) / Game::TILE_SIZE) 
					* std::round(float(wy - y) / Game::TILE_SIZE) * expl.getDamage();
	
	if (get<Game::Lifed>()->decLife(damage) > 0)
		_hurt();
	else
		killable->kill();
	expl.dealDamageTo(this);
	Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::HURT));
}


///////// BossDrawableProxy /////////
Game::BossDrawableProxy::BossDrawableProxy(const Game::Boss& b)
	: boss(b)
{}

void Game::BossDrawableProxy::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	// Assume `boss.animated` is non-null for convenience
	window.draw(*boss.animated, states);
	if (boss.hurtClock->getElapsedTime() < Game::Conf::Boss::HURT_TIME) {
		const auto& sprite = boss.animated->getSprite();
		sf::Sprite hurtSprite(*boss.animated->getTexture(), 
				sprite.getAnimation()->getFrame(sprite.getCurrentFrame()));
		hurtSprite.setOrigin(sprite.getOrigin());
		hurtSprite.setPosition(sprite.getPosition());
		hurtSprite.setRotation(sprite.getRotation());
		hurtSprite.setScale(sprite.getScale());
		hurtSprite.setColor(sf::Color(255, 0, 0, 180));
		window.draw(hurtSprite, states);
	}
}
