#include "Fire.hpp"
#include "Animated.hpp"
#include "Temporary.hpp"
#include "Drawable.hpp"
#include "core.hpp"

using Game::Fire;
using Game::TILE_SIZE;

Fire::Fire(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Time duration)
	: Game::Pond(pos, size, 1, { Game::Layers::ENEMIES, Game::Layers::ENEMIES_IGNORE_BREAKABLES })
{
	auto animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "fire.png")));
	animated->addAnimation("burn", {
		sf::IntRect(0, 0, size.x, size.y),
		sf::IntRect(TILE_SIZE, 0, size.x, size.y)
	}, true);
	animated->getTexture()->setRepeated(true);
	addComponent(new Game::Drawable(*this, *animated));
	if (duration > sf::Time::Zero) {
		sf::Clock clock;
		addComponent(new Game::Temporary(*this, [clock, duration] () {
			return clock.getElapsedTime() > duration;
		}));
	}
}
