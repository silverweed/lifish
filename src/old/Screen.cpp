#include "Screen.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "utils.hpp"
#include <cmath>

using Game::Screen;
using Game::TILE_SIZE;
using Game::WINDOW_WIDTH;
using Game::WINDOW_HEIGHT;

Screen::Screen() {
	bgSprite = addComponent(new Game::Sprite(this, 
				Game::getAsset("graphics", "screenbg1.png"), 
				sf::IntRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT)));
	bgSprite->getTexture()->setRepeated(true);
	bgSprite->getTexture()->setSmooth(true);
}

Screen::~Screen() {
	for (auto& pair : texts)
		if (pair.second != nullptr)
			delete pair.second;

	for (auto& pair : images)
		if (pair.second != nullptr)
			delete pair.second;

	for (auto& el : elements)
		delete el;
}

void Screen::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(*bgSprite, states);

	for (auto& el : elements)
		window.draw(*el, states);
	
	for (auto& pair : texts)
		window.draw(*pair.second, states);

	for (auto& pair : images)
		window.draw(*pair.second, states);
}

void Screen::triggerMouseOver(const sf::Vector2f& mousePos) {
	for (auto& pair : texts) {
		auto text = pair.second;
		if (text->getGlobalBounds().contains(mousePos.x, mousePos.y)) {
			text->setFGColor(sf::Color::Red);
		} else {
			text->setFGColor(sf::Color::White);
		}
	}

	for (auto& pair : images) {
		auto image = pair.second;
		if (image->getGlobalBounds().contains(mousePos.x, mousePos.y)) {
			image->setColor(sf::Color::Red);
		} else {
			image->setColor(sf::Color::White);
		}
	}
}

std::string Screen::triggerMouseClick(const sf::Vector2f& mousePos) {
	for (auto& pair : texts) {
		if (pair.second->getGlobalBounds().contains(mousePos.x, mousePos.y))
			return pair.first;
	}
	for (auto& pair : images) {
		if (pair.second->getGlobalBounds().contains(mousePos.x, mousePos.y))
			return pair.first;
	}
	return "";
}
