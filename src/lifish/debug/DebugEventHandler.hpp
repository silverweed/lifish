#pragma once

#include "EventHandler.hpp"

namespace lif {

class GameContext;

namespace Debug {

class DebugEventHandler : public lif::EventHandler {
	lif::GameContext& game;

public:
	DebugEventHandler(lif::GameContext& game);

	bool handleEvent(sf::Window& window, sf::Event event) override;
};

}

}
