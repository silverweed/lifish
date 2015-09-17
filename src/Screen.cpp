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
	Game::cache.loadTexture(bgTexture, Game::getAsset("test", "screenbg1.png"));

	bgTexture.setRepeated(true);
	bgTexture.setSmooth(true);

	bgSprite.setTexture(bgTexture);
	bgSprite.setTextureRect(sf::IntRect(0, 0, 2*TILE_SIZE, 2*TILE_SIZE));
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

void Screen::draw(sf::RenderTarget& window) {
	for (unsigned short i = 0, maxi = std::ceil(WINDOW_WIDTH/2.); i < maxi; ++i) {
		for (unsigned short j = 0, maxj = std::ceil(WINDOW_HEIGHT/2.); j < maxj; ++j) {
			bgSprite.setPosition(i*2*TILE_SIZE, j*2*TILE_SIZE);
			window.draw(bgSprite);
		}
	}

	for (auto& el : elements)
		window.draw(*el);
	
	for (auto& pair : texts)
		window.draw(*pair.second);

	for (auto& pair : images)
		window.draw(*pair.second);
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
