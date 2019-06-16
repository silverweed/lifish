#pragma once

#include "EventHandler.hpp"
#include <SFML/Window/Keyboard.hpp>

namespace lif {

class GameContext;

namespace debug {

class DebugEventHandler : public lif::EventHandler {
	lif::GameContext& game;

	int drawnLayerBuf = 0;
	int drawnLayerIdx = -1; // used for cycling
	bool isResetDrawnLayers = false;
	bool isAddDrawnLayers = false;
	bool changingDrawnLayers = false;

	bool _changeDrawnLayers(sf::Keyboard::Key key);
	void _finalizeChangeDrawLayers();
	void _cycleDrawnLayer();
public:
	DebugEventHandler(lif::GameContext& game);

	bool handleEvent(sf::Window& window, sf::Event event) override;
};

}

}
