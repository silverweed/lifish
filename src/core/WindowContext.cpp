#include "WindowContext.hpp"

using Game::WindowContext;

void WindowContext::handleEvents(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (handleEvent(event)) 
			return;

		for (auto& handler : handlers)
			if (handler.handleEvent(event))
				return;
	}
}
