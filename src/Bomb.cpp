#include "Bomb.hpp"
#include "Sounded.hpp"
#include "Player.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Game.hpp"

using Game::Bomb;
using Game::TILE_SIZE;
using namespace Game::Conf::Bomb;

Bomb::Bomb(const sf::Vector2f& pos, const Game::Player *const source, 
		const sf::Time& _fuseTime, const unsigned short _radius)
	: Game::Entity(pos)
	, fuseTime(_fuseTime)
	, radius(_radius)
	, sourcePlayer(source)
{
	fuseClock = addComponent(new Game::Clock(this));
	addComponent(new Game::Sounded(this, { Game::getAsset("sounds", "fuse.ogg") })); 
	killable = addComponent(new Game::Killable(this, [this] () {
		// On kill
		exploded = true;
	}, [this] () {
		// Is kill in progress
		return fuseClock->getElapsedTime() >= fuseTime;
	}));
	animated = addComponent(new Game::Animated(this, Game::getAsset("graphics", "bomb.png")));
	addComponent(new Game::Collider(this));
	addComponent(new Game::Drawable(this, animated));
	auto& a_normal_idle = animated->addAnimation("normal_idle", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	animated->addAnimation("normal_exploding", {
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	auto& a_fast_idle = animated->addAnimation("fast_idle", {
		sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE)
	});
	animated->addAnimation("fast_exploding", {
		sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE)
	});
	auto& animatedSprite = animated->getSprite();

	animatedSprite.setAnimation(fuseTime < DEFAULT_FUSE ? a_fast_idle : a_normal_idle);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	animatedSprite.play();
}

void Bomb::update() {
	Game::Entity::update();
	if (!switched && fuseTime - fuseClock->getElapsedTime() < sf::milliseconds(2000)
			&& !killable->isKilled())
	{
		animated->setAnimation(fuseTime < DEFAULT_FUSE 
				? "fast_exploding" : "normal_exploding");
		switched = true;
	}
}

void Bomb::ignite() {
	fuseTime = sf::milliseconds(50); 
	fuseClock->restart();
	ignited = true; 
}
