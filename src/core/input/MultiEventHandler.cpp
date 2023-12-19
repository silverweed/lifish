#include "MultiEventHandler.hpp"
#include "input_utils.hpp"
#include <SFML/Window.hpp>

using lif::MultiEventHandler;

void MultiEventHandler::handleEvents(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
#ifdef __APPLE__
		lif::kb::checkKeyPressed(event);
#endif

		if (handleEvent(window, event))
			return;

		for (auto& handler : handlers)
			if (handler->handleEvent(window, event))
				return;
	}
}
