#include "ControlsScreen.hpp"
#include "language.hpp"
#include "Interactable.hpp"
#include "Options.hpp"
#include "controls.hpp"
#include "game.hpp"
#include "input_utils.hpp"
#include "utils.hpp"
#include <iostream>
#include <memory>

using lif::ui::ControlsScreen;
using lif::ui::Interactable;
using Action = lif::ui::Action;
using namespace lif::controls;
using namespace std::literals::string_literals;

ControlsScreen::ControlsScreen(const sf::RenderWindow& window, const sf::Vector2u& size)
	: lif::ui::DynamicScreen(window, size)
{
	build();
}

void ControlsScreen::build() {
	name = SCREEN_NAME;
	parent = "preferences";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * P1 / P2
	 *
	 * UP: key
	 * DOWN: key
	 * LEFT: key
	 * RIGHT: key
	 * BOMB: key
	 * Joystick: no.
	 *
	 * Exit
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);

	const auto fontSize = 24;
	const float ipadx = 25,
		    ipady = 15;
	auto text = new lif::ShadedText(font, "P1", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(fontSize);
	text->setFGColor(sf::Color::Red);
	interactables["p1"] = std::make_unique<Interactable>(text);

	auto bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, " / ", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(fontSize);
	nonInteractables.emplace_back(text);

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "P2", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(fontSize);
	interactables["p2"] = std::make_unique<Interactable>(text);

	float y = bounds.top + 3 * bounds.height,
	      x = 180;
	const auto small_size = 20;
	text = new lif::ShadedText(font, lif::getLocalized("up") + ": ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font,
			lif::kb::keyToString(lif::controls::players[selectedPlayer-1][controls::CTRL_UP]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_up"] = std::make_unique<Interactable>(text);

	y += 2 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("down") + ": ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	text = new lif::ShadedText(font,
			lif::kb::keyToString(lif::controls::players[selectedPlayer-1][controls::CTRL_DOWN]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_down"] = std::make_unique<Interactable>(text);

	y += 2 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("left") + ": ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	text = new lif::ShadedText(font,
			lif::kb::keyToString(lif::controls::players[selectedPlayer-1][controls::CTRL_LEFT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_left"] = std::make_unique<Interactable>(text);

	y += 2 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("right") + ": ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	text = new lif::ShadedText(font,
			lif::kb::keyToString(lif::controls::players[selectedPlayer-1][controls::CTRL_RIGHT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_right"] = std::make_unique<Interactable>(text);

	y += 2 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("bomb") + ": ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	text = new lif::ShadedText(font,
			lif::kb::keyToString(lif::controls::players[selectedPlayer-1][controls::CTRL_BOMB]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_bomb"] = std::make_unique<Interactable>(text);

	y += 2 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("pause") + ": ", sf::Vector2f(ipadx, y));
	text->setShadowSpacing(1, 1);
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	text = new lif::ShadedText(font, "P / Esc", sf::Vector2f(x, y));
	text->setShadowSpacing(1, 1);
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	y += 3 * bounds.height;
	text = new lif::ShadedText(font, lif::getLocalized("use_joystick?"), sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.emplace_back(text);

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, lif::getLocalized("no"), sf::Vector2f(bounds.left + bounds.width + 20, y));
	text->setCharacterSize(small_size);
	interactables["joystick_toggle"] = std::make_unique<Interactable>(text);

	text = new lif::ShadedText(font, lif::getLocalized("ok"), sf::Vector2f(0, 0));
	text->setCharacterSize(fontSize);
	bounds = text->getGlobalBounds();
	sf::FloatRect win_bounds(0, 0, size.x, size.y);
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<Interactable>(text);

	_setupCallbacks();
	_setupTransitions();
}

void ControlsScreen::_setupCallbacks() {
	callbacks["p1"] = [this] () { return _selectPlayer(1); };
	callbacks["p2"] = [this] () { return _selectPlayer(2); };
	callbacks["joystick_toggle"] = [this] () { return _toggleJoystick(); };
	callbacks["change_up"] = [this] () { return _changeControl("change_up"); };
	callbacks["change_down"] = [this] () { return _changeControl("change_down"); };
	callbacks["change_right"] = [this] () { return _changeControl("change_right"); };
	callbacks["change_left"] = [this] () { return _changeControl("change_left"); };
	callbacks["change_bomb"] = [this] () { return _changeControl("change_bomb"); };
}

void ControlsScreen::_setupTransitions() {
	using D = lif::Direction;
	transitions.add("p1", std::make_pair(D::RIGHT, "p2"));
	transitions.add("p1", std::make_pair(D::DOWN, "change_up"));
	transitions.add("p2", std::make_pair(D::DOWN, "change_up"));
	transitions.add("change_up", std::make_pair(D::DOWN, "change_down"));
	transitions.add("change_down", std::make_pair(D::DOWN, "change_left"));
	transitions.add("change_left", std::make_pair(D::DOWN, "change_right"));
	transitions.add("change_right", std::make_pair(D::DOWN, "change_bomb"));
	transitions.add("change_bomb", std::make_pair(D::DOWN, "joystick_toggle"));
	transitions.add("joystick_toggle", std::make_pair(D::DOWN, "back"));
	transitions.add("back", std::make_pair(D::DOWN, "p1"));
}

Action ControlsScreen::_selectPlayer(int id) {
	if (selectedPlayer == id) return Action::DO_NOTHING;
	interactables["p" + lif::to_string(selectedPlayer)]->setColor(sf::Color::White);
	selectedPlayer = id;

	const std::array<std::pair<const char*, Control>, 5> pairs = {{
		std::make_pair("up", controls::CTRL_UP),
		std::make_pair("down", controls::CTRL_DOWN),
		std::make_pair("left", controls::CTRL_LEFT),
		std::make_pair("right", controls::CTRL_RIGHT),
		std::make_pair("bomb", controls::CTRL_BOMB)
	}};

	for (auto it = pairs.begin(); it != pairs.end(); ++it) {
		std::stringstream ss;
		ss << "change_" << it->first;
		interactables[ss.str()]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][it->second]));
	}

	const auto used = lif::controls::useJoystick[selectedPlayer-1];
	interactables["joystick_toggle"]->getText()->setString(used == -1 ? "NO" :
			"Joystick"s + lif::to_string(used));

	return Action::DO_NOTHING;
}

void ControlsScreen::update() {
	lif::ui::Screen::update();
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto& text = interactables["p" + lif::to_string(i+1)];
		if (static_cast<unsigned>(selectedPlayer) == i + 1)
			text->setColor(sf::Color::Yellow);
	}
	if (changingCtrlText != nullptr)
		changingCtrlText->setFGColor(sf::Color::Red);
}

bool ControlsScreen::handleEvent(sf::Window& win, sf::Event event) {
	if (changingCtrlText == nullptr) return Screen::handleEvent(win, event);

	const auto get_control = [this] (lif::ShadedText *text) {
		using C = lif::controls::Control;
		if (text == interactables["change_up"]->getText()) return C::CTRL_UP;
		if (text == interactables["change_down"]->getText()) return C::CTRL_DOWN;
		if (text == interactables["change_right"]->getText()) return C::CTRL_RIGHT;
		if (text == interactables["change_left"]->getText()) return C::CTRL_LEFT;
		if (text == interactables["change_bomb"]->getText()) return C::CTRL_BOMB;
		throw std::logic_error("changingCtrlText was none of the control texts!!");
	};

	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		{
			// Only meaningful if we're using a joystick (and the correct one)
			if (lif::controls::useJoystick[selectedPlayer-1] < 0 ||
				static_cast<int>(event.joystickButton.joystickId) !=
					lif::controls::useJoystick[selectedPlayer-1])
			{
				break;
			}
			lif::controls::joystickBombKey[selectedPlayer-1] = event.joystickButton.button;
			std::stringstream ss;
			ss << "Button" << event.joystickButton.button;
			changingCtrlText->setString(ss.str());
			changingCtrlText->setFGColor(sf::Color::White);
			changingCtrlText = nullptr;
			return true;
		}
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Key::Escape:
			_resyncCommandStrings();
			changingCtrlText = nullptr;
			return true;
		default:
			// Only meaningful if we're using the keyboard
			if (lif::controls::useJoystick[selectedPlayer-1] >= 0)
				break;

			lif::controls::players[selectedPlayer-1][get_control(changingCtrlText)] = event.key.code;
			changingCtrlText->setString(lif::kb::keyToString(event.key.code));
			changingCtrlText->setFGColor(sf::Color::White);
			changingCtrlText = nullptr;
			break;
		}
		break;
	default:
		break;
	}

	return Screen::handleEvent(win, event);
}

void ControlsScreen::_resyncCommandStrings() {
	interactables["change_up"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][lif::controls::CTRL_UP]));
	interactables["change_down"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][lif::controls::CTRL_DOWN]));
	interactables["change_right"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][lif::controls::CTRL_RIGHT]));
	interactables["change_left"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][lif::controls::CTRL_LEFT]));
	interactables["change_bomb"]->getText()->setString(lif::controls::useJoystick[selectedPlayer-1] >= 0
				? "Button" + lif::to_string(lif::controls::joystickBombKey[selectedPlayer-1])
				: lif::kb::keyToString(
					lif::controls::players[selectedPlayer-1][lif::controls::CTRL_BOMB]));
}

Action ControlsScreen::_changeControl(const std::string& textKey) {
	if (lif::controls::useJoystick[selectedPlayer-1] >= 0 && textKey != "change_bomb") {
		return Action::DO_NOTHING;
	}

	changingCtrlText = interactables[textKey]->getText();
	changingCtrlText->setString("_");

	return Action::DO_NOTHING;
}

Action ControlsScreen::_toggleJoystick() {
	// Retrieve the list of connected joysticks
	std::vector<int> joysticks;
	joysticks.reserve(sf::Joystick::Count);

	// Index of the currently used joystick, or -1 if no joystick is in use
	auto idx = -1;
	// Currently used joystick (-1 if none)
	auto current = lif::controls::useJoystick[selectedPlayer-1];

	for (auto i = 0; i < sf::Joystick::Count; ++i) {
		if (sf::Joystick::isConnected(i)) {
			joysticks.emplace_back(i);
			if (i == current) // found the currently used joystick
				idx = joysticks.size() - 1;
		}
	}

	if (current == -1 && joysticks.size() > 0)
		current = joysticks[0];
	else if (idx < static_cast<int>(joysticks.size()) - 1)
		current = joysticks[++idx];
	else
		current = -1;

	lif::controls::useJoystick[selectedPlayer-1] = current;
	if (current == -1) {
		interactables["joystick_toggle"]->getText()->setString("NO");
		interactables["change_up"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][controls::CTRL_UP]));
		interactables["change_down"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][controls::CTRL_DOWN]));
		interactables["change_left"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][controls::CTRL_LEFT]));
		interactables["change_right"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][controls::CTRL_RIGHT]));
		interactables["change_bomb"]->getText()->setString(lif::kb::keyToString(
				lif::controls::players[selectedPlayer-1][controls::CTRL_BOMB]));
	} else {
		std::stringstream ss;
		ss << "Joystick" << current;
		interactables["joystick_toggle"]->getText()->setString(ss.str());
		interactables["change_up"]->getText()->setString("Up");
		interactables["change_down"]->getText()->setString("Down");
		interactables["change_left"]->getText()->setString("Left");
		interactables["change_right"]->getText()->setString("Right");
		ss.str("");
		ss << "Button" << lif::controls::joystickBombKey[selectedPlayer-1];
		interactables["change_bomb"]->getText()->setString(ss.str());
	}

	return Action::DO_NOTHING;
}
