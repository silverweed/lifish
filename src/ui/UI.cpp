#include "Interactable.hpp"
#include "utils.hpp"
#include "UI.hpp"

using Game::UI::UI;

void UI::load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames) {
	screenHandler.load(window, scrNames);
}

void UI::handleEvents(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
		// In case the current screen is expecting an event, tell it.
		if (screenHandler.signalEvent(event))
			continue;

		switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::MouseButtonReleased:
			screenHandler.fireClick();
			break;
		case sf::Event::JoystickButtonPressed:
			{
				const auto btn = event.joystickButton;
				const short pb = JoystickUtils::getPauseButton(btn.joystickId);
				if (pb >= 0 && btn.button == static_cast<unsigned int>(pb))
					active = !active;
				break;
			}
		case sf::Event::KeyPressed:
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				screenHandler.setCurrentToParent();
				break;
			case sf::Keyboard::P:
				active = !active;
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
}

void UI::add(Game::UI::Screen *screen) {
	screenHandler.add(screen);
}

void UI::setOrigin(const sf::Vector2f& pos) {
	Game::WithOrigin::setOrigin(pos);
	screenHandler.setOrigin(pos);
}

void UI::update() {
	if (active)
		screenHandler.update();
}

void UI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (active)
		target.draw(screenHandler, states);
}
