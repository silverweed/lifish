#include "DroppingBomb.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Spawning.hpp"
#include "Temporary.hpp"
#include "Bomb.hpp"
#include "game.hpp"

using lif::DroppingBomb;
using lif::TILE_SIZE;

constexpr auto HEIGHT = 3 * TILE_SIZE;

DroppingBomb::DroppingBomb(const sf::Vector2f& pos, const lif::Entity *const source,
		const sf::Time& fuseTime, const unsigned short radius,
		bool isIncendiary)
	: lif::Entity(pos)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "dropping_bomb.png"));
	animated->addAnimation("drop", {
		sf::IntRect(0 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(1 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(7 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(8 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(9 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
		sf::IntRect(10 * TILE_SIZE, 0, TILE_SIZE, HEIGHT),
	}, true);
	animated->getSprite().setLooped(false);
	animated->getSprite().setFrameTime(sf::seconds(0.11));
	addComponent<lif::Drawable>(*this, *animated);
	spawning = addComponent<lif::Spawning>(*this, [this] (const lif::Spawning& spw) {
		// spawn condition
		return !animated->getSprite().isPlaying() && spw.nSpawned() < 1;
	}, [=] () {
		return new lif::Bomb(position + sf::Vector2f(0, 2 * TILE_SIZE),
				source, fuseTime, radius, isIncendiary);
	});
	addComponent<lif::Temporary>(*this, [this] () {
		return spawning->nSpawned() > 0;
	});
}
