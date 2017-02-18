#include "Torch.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "LightSource.hpp"
#include "ZIndexed.hpp"
#include "core.hpp"
#include "conf/zindex.hpp"

using lif::Torch;

Torch::Torch(const sf::Vector2f& pos, unsigned tileRadius)
	: lif::Entity(pos)
{
	auto animated = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "torch.png")));
	animated->addAnimation("burn", {
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE)
	}, true);
	animated->getSprite().setFrameTime(sf::milliseconds(100));
	addComponent(std::make_shared<lif::Drawable>(*this, *animated));
	addComponent(std::make_shared<lif::LightSource>(*this, 
			tileRadius * lif::TILE_SIZE, sf::Color(150, 150, 150), 0.8, 20));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::TORCHES));
}
