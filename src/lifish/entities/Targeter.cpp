#include "Targeter.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "core.hpp"

using lif::Targeter;

Targeter::Targeter(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "targeter.png"));
	animated->addAnimation("blink", {
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(4 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
	}, true);
	animated->getSprite().setFrameTime(sf::milliseconds(50));
	animated->getSprite().play();
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::FLASHES);
}
