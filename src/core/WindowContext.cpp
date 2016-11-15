#include "WindowContext.hpp"
#include <SFML/Window.hpp>

using Game::WindowContext;

void WindowContext::handleEvents(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (handleEvent(window, event)) 
			return;

		for (auto& handler : handlers)
			if (handler->handleEvent(window, event))
				return;
	}
}
