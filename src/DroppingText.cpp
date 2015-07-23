#include "DroppingText.hpp"

using Game::DroppingText;

DroppingText::DroppingText(const std::string& texture_name, const sf::Vector2i& texture_rect, float _speed) :
	Game::Temporary(sf::Vector2f(0.f, -50.f), texture_name),
	speed(_speed)
{
	pos.x = (Game::TILE_SIZE*(Game::LEVEL_WIDTH + 2) - texture_rect.x) / 2;
	sprite.setTextureRect(sf::IntRect(0, 0, texture_rect.x, texture_rect.y));	
}

void DroppingText::draw(sf::RenderTarget& window) {
	if (!stopped && pos.y >= Game::LEVEL_HEIGHT * Game::TILE_SIZE / 2) {
		stopped = true;
		pauseClock.restart();
	}
	if (!stopped || pauseClock.getElapsedTime().asSeconds() >= 1.) {
		sf::Time frameTime = frameClock.restart();
		pos.y += speed * frameTime.asSeconds();
		sprite.setPosition(pos);
	} else {
		frameClock.restart();
	}
	window.draw(sprite);
}
