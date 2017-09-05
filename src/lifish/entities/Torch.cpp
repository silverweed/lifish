#include "Torch.hpp"
#include "Animated.hpp"
#include "Level.hpp"
#include "Drawable.hpp"
#include "LightSource.hpp"
#include "ZIndexed.hpp"
#include "entity_type.hpp"
#include "core.hpp"
#include "conf/zindex.hpp"

using lif::Torch;

Torch::Torch(const sf::Vector2f& pos, unsigned tileRadius)
	: lif::Entity(pos)
{
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "torch.png"));
	animated->addAnimation("burn", {
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE)
	}, true);
	animated->addAnimation("off", {
		sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE)
	});
	animated->getSprite().setFrameTime(sf::milliseconds(100));
	auto drawable = addComponent<lif::Drawable>(*this, *animated);
	drawable->setScaleOrigin(lif::TILE_SIZE / 2, lif::TILE_SIZE / 2);
	addComponent<lif::LightSource>(*this, tileRadius * lif::TILE_SIZE, sf::Color(150, 150, 150), 0.8, 20);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TORCHES);
}

void Torch::fixOrientation(const lif::Level& level) {
	fixOrientation(level.getTilemapRaw(), level.getInfo().width);
}

void Torch::fixOrientation(const std::string& tilemap, int lvWidth) {
	const int left = position.x / lif::TILE_SIZE - 1,
	          top = position.y / lif::TILE_SIZE - 1;

	float scaleX = 1;

	// If it has a level limit nearby, tether to it
	if (left == 0)
		goto end;
	if (left == lvWidth - 1)
		goto flip;

	// If it has a wall on its left, tether to left
	switch (lif::entityFromLetter(tilemap[top * lvWidth + left - 1])) {
	case EntityType::BREAKABLE:
	case EntityType::FIXED:
	case EntityType::TRANSPARENT_WALL:
		goto end;
	default:
		break;
	}

	// If it has a wall on its right, tether to right
	switch (lif::entityFromLetter(tilemap[top * lvWidth + left + 1])) {
	case EntityType::BREAKABLE:
	case EntityType::FIXED:
	case EntityType::TRANSPARENT_WALL:
		goto flip;
	default:
		break;
	}

	// Else, give up and tether to whichever tile it has at its left
	goto end;

flip:
	scaleX = -1;
end:
	get<lif::Drawable>()->setScale(scaleX, 1);
}
