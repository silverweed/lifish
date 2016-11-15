#include "Interactable.hpp"
#include "utils.hpp"
#include "UI.hpp"
#include "Screen.hpp"
#include "BaseEventHandler.hpp"
#include "screen_callbacks.hpp"
#include <exception>
#include <iostream>

using Game::UI::UI;

UI::UI() : Game::WindowContext() {
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::BaseEventHandler));
}

void UI::load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames) {
	for (const auto& name : scrNames) {
		if (screens.find(name) != screens.end()) {
			std::cerr << "[ WARNING ] Screen " << name << " already loaded: skipping." << std::endl;	
			continue;
		}
		auto screen = new Game::UI::Screen(name, window, size);
		if (curScreen == nullptr) {
			curScreen = screen;
			curScreen->setOrigin(origin);
		}
		screens[screen->getName()] = std::unique_ptr<Game::UI::Screen>(screen);
	}
}

bool UI::handleEvent(sf::Window& window, sf::Event event) {
	if (curScreen != nullptr && curScreen->handleEvent(window, event))
		return true;

	// In case the current screen is expecting an event, tell it.
	switch (event.type) {
	case sf::Event::MouseButtonReleased:
		fireClick();
		return true;
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			const short pb = Game::JoystickUtils::getPauseButton(btn.joystickId);
			if (pb >= 0 && btn.button == static_cast<unsigned int>(pb))
				active = !active;
			return true;
		}
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Escape:
			setCurrentToParent();
			return true;
		case sf::Keyboard::P:
			active = !active;
			return true;
		default:
			break;
		}
	default:
		break;
	}
	return false;
}

void UI::add(Game::UI::Screen *screen) {
	screens[screen->getName()] = std::unique_ptr<Game::UI::Screen>(screen); 
}

void UI::setOrigin(const sf::Vector2f& pos) {
	Game::WithOrigin::setOrigin(pos);
}

void UI::update() {
	if (curScreen != nullptr)
		curScreen->update();
}

void UI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (curScreen != nullptr)
		target.draw(*curScreen, states);
}

void UI::fireClick() {
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
		active = false;
		break;
	default:
		break;
	}
}

void UI::setCurrent(const std::string& name, bool overrideParent) {
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

void UI::setCurrentToParent() {
	setCurrent(curScreen->getParent());
}
