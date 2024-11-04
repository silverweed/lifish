#include "UI.hpp"
#include "BaseEventHandler.hpp"
#include "Interactable.hpp"
#include "SaveScreen.hpp"
#include "Screen.hpp"
#include "ScreenBuilder.hpp"
#include "contexts.hpp"
#include "input_utils.hpp"
#include "screen_callbacks.hpp"
#include "utils.hpp"
#include <exception>
#include <iostream>

using lif::ui::UI;

UI::UI() : lif::WindowContext() {
	_addHandler<lif::BaseEventHandler>();
}

void UI::load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames) {
	for (const auto& name : scrNames) {
		if (screens.find(name) != screens.end()) {
			std::cerr << "[ WARNING ] Screen " << name << " already loaded: skipping." << std::endl;
			continue;
		}
		ScreenBuilder builder { dynamicTexts };
		auto screen = std::make_unique<lif::ui::Screen>(name, window, size, builder);
		if (curScreen == nullptr) {
			curScreen = screen.get();
			curScreen->setOrigin(origin);
		}
		screens[screen->getName()] = std::move(screen);
	}
}

bool UI::handleEvent(sf::Window& window, sf::Event event) {
	if (curScreen == nullptr)
		return false;

	if (curScreen->pollTriggeredAction()) {
		_processAction(curScreen->getTriggeredAction());
		return true;
	}

	if (curScreen->handleEvent(window, event))
		return true;

	// In case the current screen is expecting an event, tell it.
	switch (event.type) {
	case sf::Event::MouseButtonReleased:
		fireClick();
		return true;
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			if (lif::joystick::isButton(lif::joystick::ButtonType::START, btn.joystickId, btn.button)) {
				if (getCurrent() == "pause")
					newContext = lif::CTX_GAME;
			} else if (curScreen->getLastSelectMethod() == Screen::SelectMethod::JOYSTICK) {
				if (lif::joystick::isButton(
						lif::joystick::ButtonType::BTN_DOWN, btn.joystickId, btn.button))
				{
					fireClick();
				} else if (lif::joystick::isButton(
						lif::joystick::ButtonType::BTN_RIGHT, btn.joystickId, btn.button))
				{
					setCurrentToParent();
				}
			}
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
		case sf::Keyboard::Space:
		case sf::Keyboard::Return:
			if (curScreen->getLastSelectMethod() == Screen::SelectMethod::KEYBOARD) {
				fireClick();
				return true;
			}
			break;
		case sf::Keyboard::Backspace:
			setCurrentToParent();
			return true;
		default:
			break;
		}
	default:
		break;
	}
	return false;
}

void UI::setOrigin(const sf::Vector2f& pos) {
	lif::WithOrigin::setOrigin(pos);
}

void UI::update() {
	quitGame = false;
	loadGame = false;
	saveGame = false;
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

	lif::ui::Action action;

	// Check if screen has an internal registered callback for that element; if not, call an external one.
	if (curScreen->hasCallback(cbname)) {
		action = curScreen->fireCallback(cbname);
	} else {
		auto it = lif::ui::screenCallbacks.find(cbname);
		if (it == lif::ui::screenCallbacks.end()) return;
		action = it->second();
	}

	_processAction(action);
}

void UI::_processAction(lif::ui::Action action) {
	switch (action) {
	case Action::EXIT:
		// terminate the game
		lif::exitCode = 0;
		lif::terminated = true;
		break;
	case Action::SAVE_GAME:
		saveGame = true;
		saveName = lif::getSaveDir() + static_cast<lif::ui::SaveScreen*>(curScreen)->getPrompt();
		setCurrentToParent();
		break;
	case Action::LOAD_GAME:
		loadGame = true;
		newContext = lif::CTX_INTERLEVEL;
		break;
	case Action::QUIT_TO_HOME:
		quitGame = true;
		setCurrent("home");
		break;
	case Action::SWITCH_SCREEN:
		setCurrent(lif::ui::screenCallbackArg);
		break;
	case Action::SWITCH_SCREEN_OVERRIDE_PARENT:
		setCurrent(lif::ui::screenCallbackArg, true);
		break;
	case Action::SWITCH_TO_PARENT:
		setCurrentToParent();
		break;
	case Action::START_GAME:
		newContext = lif::CTX_INTERLEVEL;
		break;
	case Action::DEACTIVATE_UI:
		newContext = lif::CTX_GAME;
		break;
	default:
		break;
	}
}

void UI::setCurrent(const std::string& name, bool overrideParent) {
	if (name.length() == 0) return;

	curScreen->onUnload();
	const auto oldScreen = curScreen->getName();
	const auto it = screens.find(name);
	if (it == screens.end())
		throw std::invalid_argument("Screen " + name + " was not loaded!");
	curScreen = it->second.get();
	curScreen->setOrigin(origin);
	if (overrideParent)
		curScreen->setParent(oldScreen);
	curScreen->onLoad();
}

void UI::setCurrentToParent() {
	setCurrent(curScreen->getParent());
}

std::string UI::getCurrent() const {
	if (curScreen == nullptr) return "";
	return curScreen->getName();
}

void UI::setDynamicText(const std::string& name, const std::string& value) {
	dynamicTexts[name] = value;
}

void UI::rebuildScreens() {
	for (const auto& screenPair : screens) {
		auto& screen = screenPair.second;
		lif::ui::ScreenBuilder builder { dynamicTexts };
		screen->rebuild(builder);
	}
}
