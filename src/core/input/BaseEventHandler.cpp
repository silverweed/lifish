#include "BaseEventHandler.hpp"
#include "Options.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>

using lif::BaseEventHandler;

bool BaseEventHandler::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::Closed:
		lif::terminated = true;
		return true;
	case sf::Event::Resized:
		static_cast<sf::RenderWindow&>(window).setView(lif::keepRatio(
			sf::Vector2f(event.size.width, event.size.height), lif::options.windowSize));
		return true;
	case sf::Event::KeyPressed:
		switch (event.key.code) {
#ifndef RELEASE
		case sf::Keyboard::V:
			lif::options.vsync = !lif::options.vsync;
			window.setFramerateLimit(lif::options.vsync ? lif::options.framerateLimit : 0);
			window.setVerticalSyncEnabled(lif::options.vsync);
			return true;
		case sf::Keyboard::Q:
			lif::terminated = true;
			return true;
#endif
		default:
			break;
		}
	default:
		break;
	}
	return false;
}
