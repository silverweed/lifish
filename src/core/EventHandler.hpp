#pragma once

#include <SFML/Window/Window.hpp>

namespace Game {

class EventHandler {
public:
	/** Handles a single event.
	 *  @return true if the event was caught.
	 */
	virtual bool handleEvent(sf::Window& window, sf::Event evt) = 0;
};

}
