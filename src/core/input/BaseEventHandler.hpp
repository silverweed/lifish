#pragma once

#include <array>
#include <SFML/Window.hpp>
#include "EventHandler.hpp"

namespace sf {
	class View;
}

namespace lif {

class BaseEventHandler : public lif::EventHandler {
public:
	bool handleEvent(sf::Window& window, sf::Event event) override;
};

}
