#pragma once

#include <deque>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Window.hpp>
#include "EventHandler.hpp"
#include "WithOrigin.hpp"
#include "Activable.hpp"

namespace Game {

/**
 * A WindowContext defines what portion of the game is currently active
 * and displayed. This includes the event loop, logic loop and rendering loop.
 * Only one context should be active at a time.
 */
class WindowContext
	: public sf::Drawable
	, public Game::WithOrigin
	, public Game::Activable
	, public Game::EventHandler
{
protected:
	/** The chain of fallback handlers.
	 *  If this handler didn't catch any event, it may delegate said event to other handlers,
	 *  which will try sequentially to catch it.
	 *  Useful to share and reuse functionality of event handlers.
	 */
	std::deque<std::unique_ptr<Game::EventHandler>> handlers;

public:
	virtual void update() = 0;
	void handleEvents(sf::Window& window);
};

}
