#include "DroppingSprite.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::DroppingSprite;

DroppingSprite::DroppingSprite(const std::string& texture_name, const sf::Vector2i& texture_rect, float speed)
	: lif::Entity(sf::Vector2f(
		(lif::SIDE_PANEL_WIDTH + lif::GAME_WIDTH - 2 * lif::TILE_SIZE - texture_rect.x) / 2,
		-texture_rect.y))
	, origPosition(position)
	, height(texture_rect.y / 2.)
{
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::DROPPING_TEXTS);
	auto sprite = addComponent<lif::Sprite>(*this, texture_name,
			sf::IntRect(0, 0, texture_rect.x, texture_rect.y));

	addComponent<lif::Drawable>(*this, *sprite);
	moving = addComponent<lif::AxisMoving>(*this, speed);
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

	pauseT += lif::time.getDelta();

	if (position.y > lif::GAME_HEIGHT + 2 * lif::TILE_SIZE) {
		reset();
	} else if (stoppedAtMiddle) {
		if (pauseT >= PAUSE_TIME)
			moving->setDirection(lif::Direction::DOWN);
	} else if (position.y + height / 2 >= lif::GAME_HEIGHT / 2 - lif::TILE_SIZE) {
		stoppedAtMiddle = true;
		moving->stop();
		pauseT = sf::Time::Zero;
	}
}
