#pragma once

#include <vector>
#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include "EventHandler.hpp"

namespace lif {

/**
 * A MultiEventHandler is an EventHandler which consumes all Events available. It can implement the event
 * handling logic itself or delegate it to other EventHandlers.
 */
class MultiEventHandler : public lif::EventHandler, private sf::NonCopyable {
protected:
	/** The chain of fallback handlers.
	 *  If this handler didn't catch any event, it may delegate said event to other handlers,
	 *  which will try sequentially to catch it.
	 *  Useful to share and reuse functionality of event handlers.
	 */
	std::vector<std::unique_ptr<lif::EventHandler>> handlers;

	template<class T, class... Args>
	void _addHandler(Args&&... args) {
		handlers.emplace_back(new T(std::forward<Args>(args)...));
	}
public:
	void handleEvents(sf::Window& window);
	virtual bool handleEvent(sf::Window&, sf::Event) override { return false; }
};

}
