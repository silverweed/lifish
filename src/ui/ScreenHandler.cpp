#include "ScreenHandler.hpp"
#include "screen_callbacks.hpp"
#include "UI.hpp"
#include "game.hpp"
#include <iostream>

using Game::UI::ScreenHandler;
using Game::UI::Action;

void ScreenHandler::load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames) {
	for (const auto& name : scrNames) {
		if (screens.find(name) != screens.end()) {
			std::cerr << "[ WARNING ] Screen " << name << " already loaded: skipping." << std::endl;	
			continue;
		}
		auto screen = new Game::UI::Screen(name, window);
		if (curScreen == nullptr) {
			curScreen = screen;
			curScreen->setOrigin(origin);
		}
		screens[screen->getName()] = std::unique_ptr<Game::UI::Screen>(screen);
	}
} 

void ScreenHandler::add(Game::UI::Screen *screen) {
	screens[screen->getName()] = std::unique_ptr<Game::UI::Screen>(screen); 
}

void ScreenHandler::fireClick() {
	if (curScreen == nullptr) return;
	auto cbname = curScreen->getSelected();

	Game::UI::Action action;

	// Check if screen has an internal registered callback for that element; if not, call an external one.
	if (curScreen->hasCallback(cbname)) {
		action = curScreen->fireCallback(cbname);
	} else {
		auto it = Game::UI::screenCallbacks.find(cbname);
		if (it == Game::UI::screenCallbacks.end()) return;
		action = it->second();
	}

	switch (action) {
	case Action::EXIT:
		// terminate the game
		Game::exitCode = 0;
		Game::terminated = true;
		break;
	case Action::START_GAME:
		// TODO
		break;
	case Action::SAVE_GAME:
		// TODO
		break;
	case Action::SWITCH_SCREEN:
		setCurrent(Game::UI::screenCallbackArg);
		break;
	case Action::SWITCH_SCREEN_OVERRIDE_PARENT:
		setCurrent(Game::UI::screenCallbackArg, true);
		break;
	case Action::SWITCH_TO_PARENT:
		setCurrentToParent();
		break;
	case Action::DEACTIVATE_UI:
		ui.setActive(false);
		break;
	default:
		break;
	}
}

void ScreenHandler::setCurrent(const std::string& name, bool overrideParent) {
	if (name.length() == 0) return;

	auto oldScreen = curScreen->getName();
	auto it = screens.find(name);
	if (it == screens.end())
		throw std::invalid_argument("Screen " + name + " was not loaded!");
	curScreen = it->second.get();
	curScreen->setOrigin(origin);
	if (overrideParent)
		curScreen->setParent(oldScreen);
}

void ScreenHandler::setCurrentToParent() {
	setCurrent(curScreen->getParent());
}
