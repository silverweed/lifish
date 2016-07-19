#include "UI.hpp"

using Game::UI::UI;

void UI::load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames) {
	screenHandler.load(window, scrNames);
}

void UI::handleEvents(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::MouseButtonReleased:
			screenHandler.fireClick();
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				screenHandler.setCurrentToParent();
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
}
