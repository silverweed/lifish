#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include "MultiEventHandler.hpp"
#include "WithOrigin.hpp"
#include "Activable.hpp"

namespace lif {

/**
 * A WindowContext defines what portion of the game is currently active
 * and displayed. This includes the event loop, logic loop and rendering loop.
 * Only one context should be active at a time.
 */
class WindowContext
	: public sf::Drawable
	, public lif::WithOrigin
	, public lif::Activable
	, public lif::MultiEventHandler
{
protected:
	int newContext = -1;

public:
	/** When this method returns a non-negative number, the current WindowContext
	 *  should be switched with that indexed by the returned number.
	 *  The number=>context association is game-dependent, and should be defined
	 *  per-case with an enum or something alike.
	 */
	int getNewContext() const { return newContext; }
	/** This must be called after switching context to reset the `newContext` variable. */
	void resetNewContext() { newContext = -1; }

	virtual void update() = 0;
};

}
