#include "DroppingSprite.hpp"
#include "Sprite.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::DroppingSprite;

DroppingSprite::DroppingSprite(const std::string& texture_name, const sf::Vector2i& texture_rect, float speed)
	: lif::Entity(sf::Vector2f(
		(lif::SIDE_PANEL_WIDTH + lif::TILE_SIZE * lif::LEVEL_WIDTH - texture_rect.x) / 2,
		-texture_rect.y))
	, origPosition(position)
	, height(texture_rect.y / 2.)
{
	addComponent(new lif::ZIndexed(*this, lif::conf::zindex::DROPPING_TEXTS));
	auto sprite = addComponent(new lif::Sprite(*this, texture_name, 
				sf::IntRect(0, 0, texture_rect.x, texture_rect.y)));

	pauseClock = addComponent(new lif::Clock(*this));
	addComponent(new lif::Drawable(*this, *sprite));
	moving = addComponent(new lif::AxisMoving(*this, speed));
	moving->setAutoRealign(false);
	moving->setEnsureAlign(false);
}

void DroppingSprite::play() {
	moving->setDirection(lif::Direction::DOWN);
	playing = true;
}

void DroppingSprite::reset() {
	moving->stop();
	position = origPosition;
	playing = stoppedAtMiddle = false;
}

void DroppingSprite::update() {
	lif::Entity::update();
	if (position.y > (lif::LEVEL_HEIGHT + 2) * lif::TILE_SIZE) {
		reset();
	} else if (stoppedAtMiddle) {
		if (pauseClock->getElapsedTime() >= PAUSE_TIME)
			moving->setDirection(lif::Direction::DOWN);
	} else if (position.y + height / 2 >= lif::LEVEL_HEIGHT * lif::TILE_SIZE / 2) {
		stoppedAtMiddle = true;
		moving->stop();
		pauseClock->restart();
	}
}
