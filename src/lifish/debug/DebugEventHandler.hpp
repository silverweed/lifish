#pragma once

#include "EventHandler.hpp"

namespace Game {

class GameContext;

namespace Debug {

class DebugEventHandler : public Game::EventHandler {
	Game::GameContext& game;

public:
	DebugEventHandler(Game::GameContext& game);

	bool handleEvent(sf::Window& window, sf::Event event) override;
};

}

}
