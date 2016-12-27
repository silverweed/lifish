#include "DroppingSprite.hpp"
#include "Sprite.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "zindex.hpp"
#include "game.hpp"

using Game::DroppingSprite;

DroppingSprite::DroppingSprite(const std::string& texture_name, const sf::Vector2i& texture_rect, float speed)
	: Game::Entity(sf::Vector2f(
		(Game::SIDE_PANEL_WIDTH + Game::TILE_SIZE * Game::LEVEL_WIDTH - texture_rect.x) / 2,
		-texture_rect.y))
	, origPosition(position)
	, height(texture_rect.y / 2.)
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::DROPPING_TEXTS));
	auto sprite = addComponent(new Game::Sprite(*this, texture_name, 
				sf::IntRect(0, 0, texture_rect.x, texture_rect.y)));

	pauseClock = addComponent(new Game::Clock(*this));
	addComponent(new Game::Drawable(*this, *sprite));
	moving = addComponent(new Game::AxisMoving(*this, speed));
	moving->setAutoRealignEnabled(false);
	moving->setEnsureAlignEnabled(false);
}

void DroppingSprite::play() {
	moving->setDirection(Game::Direction::DOWN);
	playing = true;
}

void DroppingSprite::reset() {
	moving->stop();
	position = origPosition;
	playing = stoppedAtMiddle = false;
}

void DroppingSprite::update() {
	Game::Entity::update();
	if (position.y > (Game::LEVEL_HEIGHT + 2) * Game::TILE_SIZE) {
		reset();
	} else if (stoppedAtMiddle) {
		if (pauseClock->getElapsedTime() >= PAUSE_TIME)
			moving->setDirection(Game::Direction::DOWN);
	} else if (position.y + height / 2 >= Game::LEVEL_HEIGHT * Game::TILE_SIZE / 2) {
		stoppedAtMiddle = true;
		moving->stop();
		pauseClock->restart();
	}
}
