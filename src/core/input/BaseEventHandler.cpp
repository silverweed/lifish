#include "BaseEventHandler.hpp"
#include "Options.hpp"
#include "core.hpp"
#include <SFML/Graphics.hpp>

using lif::BaseEventHandler;

bool BaseEventHandler::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::Closed:
		lif::terminated = true;
		return true;
	case sf::Event::Resized:
		static_cast<sf::RenderWindow&>(window).setView(_keepRatio(event.size, lif::options.windowSize));
		return true;
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::V:
			lif::options.vsync = !lif::options.vsync;
			window.setFramerateLimit(lif::options.vsync ? lif::options.framerateLimit : 0);
			window.setVerticalSyncEnabled(lif::options.vsync);
			return true;
#ifndef RELEASE
		case sf::Keyboard::Q:
			lif::terminated = true;
			return true;
		case sf::Keyboard::Num0:
			lif::options.printDrawStats = !lif::options.printDrawStats;
			return true;
#endif
		default:
			break;
		}
	default:
		break;
	}
	return false;
}

sf::View BaseEventHandler::_keepRatio(const sf::Event::SizeEvent& size, const sf::Vector2u& designedsize) {
	sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

	const float screenwidth = size.width / static_cast<float>(designedsize.x),
	            screenheight = size.height / static_cast<float>(designedsize.y);

	if (screenwidth > screenheight) {
		viewport.width = screenheight / screenwidth;
		viewport.left = (1.f - viewport.width) / 2.f;
	} else if (screenwidth < screenheight) {
		viewport.height = screenwidth / screenheight;
		viewport.top = (1.f - viewport.height) / 2.f;
	}

	sf::View view(sf::FloatRect(0, 0, designedsize.x, designedsize.y));
	view.setViewport(viewport);

	return view;
}

