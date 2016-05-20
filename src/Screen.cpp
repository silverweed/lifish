#include "Screen.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Clickable.hpp"
#include "utils.hpp"
#include <cmath>

using Game::Screen;
using Game::TILE_SIZE;

Screen::Screen() : Game::Entity() {
	bgSprite = addComponent(new Game::Sprite(this, 
				Game::getAsset("graphics", "screenbg1.png"), 
				sf::IntRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT)));
	bgSprite->getTexture()->setRepeated(true);
	bgSprite->getTexture()->setSmooth(true);
}

//Screen::~Screen() {
	//for (auto& pair : texts)	
		//if (pair.second != nullptr)
			//delete pair.second;
	//for (auto& pair : images)	
		//if (pair.second != nullptr)
			//delete pair.second;
	//for (auto& e : elements)
		//if (e != nullptr)
			//delete e;
//}

void Screen::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(*bgSprite, states);

	for (auto& el : elements)
		window.draw(*el, states);
	
	for (auto& pair : clickables)
		window.draw(pair.second, states);
}

void Screen::triggerMouseOver(const sf::Vector2f& mousePos) {
	for (auto& pair : clickables) {
		auto& c = pair.second;
		c.highlight(c.isBelow(mousePos));
	}
}

std::string Screen::triggerMouseClick(const sf::Vector2f& mousePos) {
	for (auto& pair : clickables) {
		if (pair.second.isBelow(mousePos))
			return pair.first;
	}
	return "";
}

void Screen::_addClickable(const std::string& key, Game::ShadedText *elem) {
	clickables.emplace(key, Game::Clickable(elem, [elem] (const sf::Vector2f& pos) {
		return elem->getGlobalBounds().contains(pos.x, pos.y);
	}, [elem] (bool on) {
		elem->setFGColor(on ? sf::Color::Red : sf::Color::White);	
	}));
}

void Screen::_addClickable(const std::string& key, Game::Sprite *elem) {
	clickables.emplace(key, Game::Clickable(elem, [elem] (const sf::Vector2f& pos) {
		return elem->getSprite().getGlobalBounds().contains(pos.x, pos.y);
	}, [elem] (bool on) {
		elem->getSprite().setColor(on ? sf::Color::Red : sf::Color::White);	
	}));
}
