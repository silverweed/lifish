#include "DroppingText.hpp"
#include "Temporary.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "game_values.hpp"

using Game::DroppingText;

DroppingText::DroppingText(const std::string& texture_name, const sf::Vector2i& texture_rect, float speed)
	: Game::Entity(sf::Vector2f(0.f, -texture_rect.y))
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::DROPPING_TEXTS));
	auto sprite = addComponent(new Game::Sprite(*this, texture_name, 
				sf::IntRect(0, 0, texture_rect.x, texture_rect.y)));
	height = texture_rect.y/2.0;
	position.x = (Game::TILE_SIZE*(Game::LEVEL_WIDTH + 2) - texture_rect.x) / 2;
	position.y = -Game::TILE_SIZE;
	pauseClock = addComponent(new Game::Clock(*this));
	pauseClock->pause();
	addComponent(new Game::Drawable(*this, *sprite));
	moving = addComponent(new Game::AxisMoving(*this, speed));
	addComponent(new Game::Temporary(*this, [this] () {
		// expire condition
		return position.y > (Game::LEVEL_HEIGHT + 2) * Game::TILE_SIZE;
	}));
}

void DroppingText::play() {
	moving->setDirection(Game::Direction::DOWN);
}

void DroppingText::update() {
	Game::Entity::update();
	if (stoppedAtMiddle) {
		if (pauseClock->getElapsedTime() >= PAUSE_TIME)
			moving->setDirection(Game::Direction::DOWN);
	} else if (position.y + height/2 >= Game::LEVEL_HEIGHT * Game::TILE_SIZE / 2) {
		stoppedAtMiddle = true;
		moving->stop();
		pauseClock->resume();
	}
}
