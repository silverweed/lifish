#include "Fire.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "LightSource.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "core.hpp"

using lif::Fire;
using lif::TILE_SIZE;

Fire::Fire(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Time duration)
	: lif::Pond(pos, size, 1, { lif::c_layers::ENEMIES, lif::c_layers::ENEMIES_IGNORE_BREAKABLES })
{
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "fire.png"));
	animated->addAnimation("burn", {
		sf::IntRect(0, 0, size.x, size.y),
		sf::IntRect(TILE_SIZE, 0, size.x, size.y)
	}, true);
	animated->getTexture()->setRepeated(true);
	addComponent<lif::LightSource>(*this, 20, sf::Color(244, 152, 56), 0.7, 15);
	addComponent<lif::Drawable>(*this, *animated);
	if (duration > sf::Time::Zero) {
		const auto now = lif::time.getGameTime();
		addComponent<lif::Temporary>(*this, [now, duration] () {
			return lif::time.getGameTime() > now + duration;
		});
	}
}
