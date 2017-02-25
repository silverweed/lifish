#pragma once

#include <SFML/Window/Window.hpp>

namespace lif {

class EventHandler {
public:
	virtual ~EventHandler() {}

	/** Handles a single event.
	 *  @return true if the event was caught.
	 */
	virtual bool handleEvent(sf::Window& window, sf::Event evt) = 0;
};

}
