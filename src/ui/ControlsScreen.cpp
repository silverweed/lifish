#include "ControlsScreen.hpp"
#include "game.hpp"
#include "Options.hpp"
#include "utils.hpp"
#include <memory>
#include <iostream>

using Game::UI::ControlsScreen;
using Game::UI::Interactable;
using Action = Game::UI::Action;
using namespace Game::Controls;

ControlsScreen::ControlsScreen(const sf::RenderWindow& window) 
	: Game::UI::Screen(window) 
{
	name = "controls";
	parent = "preferences";
	_loadBGSprite(Game::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 * 
	 * P1 / P2
	 *
	 * UP: key
	 * DOWN: key
	 * LEFT: key
	 * RIGHT: key
	 * BOMB: key
	 *
	 * Exit
	 */
	const auto font = Game::getAsset("fonts", Game::Fonts::SCREEN);

	const auto fontSize = 24;
	const float ipadx = 25,
		    ipady = 15;
	auto text = new Game::ShadedText(font, "P1", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(fontSize);
	text->setFGColor(sf::Color::Red);
	interactables["p1"] = std::unique_ptr<Interactable>(new Interactable(text));

	auto bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, " / ", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(fontSize);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "P2", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(fontSize);
	interactables["p2"] = std::unique_ptr<Interactable>(new Interactable(text));

	float y = bounds.top + 3 * bounds.height,
	      x = 180;
	const auto small_size = 20;
	text = new Game::ShadedText(font, "UP: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Controls::CTRL_UP]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_up"] = std::unique_ptr<Interactable>(new Interactable(text));

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "DOWN: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Controls::CTRL_DOWN]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_down"] = std::unique_ptr<Interactable>(new Interactable(text));

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "LEFT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Controls::CTRL_LEFT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_left"] = std::unique_ptr<Interactable>(new Interactable(text));

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "RIGHT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Controls::CTRL_RIGHT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_right"] = std::unique_ptr<Interactable>(new Interactable(text));

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "BOMB: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Controls::CTRL_BOMB]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	interactables["change_bomb"] = std::unique_ptr<Interactable>(new Interactable(text));

	y += 3 * bounds.height;
	text = new Game::ShadedText(font, "Use Joystick?", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "NO", sf::Vector2f(bounds.left + bounds.width + 20, y));
	text->setCharacterSize(small_size);
	interactables["joystick_toggle"] = std::unique_ptr<Interactable>(new Interactable(text));

	text = new Game::ShadedText(font, "OK", sf::Vector2f(0, 0));
	text->setCharacterSize(fontSize);
	bounds = text->getGlobalBounds();
	sf::FloatRect win_bounds(0, 0, size.x, size.y);
	text->setPosition(sf::Vector2f(Game::center(bounds, win_bounds).x, win_bounds.height - 2 * bounds.height));
	interactables["back"] = std::unique_ptr<Interactable>(new Interactable(text));

	// Setup internal callbacks
	callbacks["p1"] = [this] () { return _selectPlayer(1); };
	callbacks["p2"] = [this] () { return _selectPlayer(2); };
	callbacks["joystick_toggle"] = [this] () { return _toggleJoystick(); };
	callbacks["change_up"] = [this] () { return _changeControl("change_up"); };
	callbacks["change_down"] = [this] () { return _changeControl("change_down"); };
	callbacks["change_right"] = [this] () { return _changeControl("change_right"); };
	callbacks["change_left"] = [this] () { return _changeControl("change_left"); };
	callbacks["change_bomb"] = [this] () { return _changeControl("change_bomb"); };
}

Action ControlsScreen::_selectPlayer(unsigned short id) {
	if (selectedPlayer == id) return Action::DO_NOTHING;
	interactables["p" + Game::to_string(selectedPlayer)]->setColor(sf::Color::White);
	selectedPlayer = id;
	
	const std::array<std::pair<const char*, Control>, 5> pairs = {{
		std::make_pair("up", Controls::CTRL_UP),
		std::make_pair("down", Controls::CTRL_DOWN),
		std::make_pair("left", Controls::CTRL_LEFT),
		std::make_pair("right", Controls::CTRL_RIGHT),
		std::make_pair("bomb", Controls::CTRL_BOMB)
	}};

	for (auto it = pairs.begin(); it != pairs.end(); ++it) {
		std::stringstream ss;
		ss << "change_" << it->first;
		interactables[ss.str()]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][it->second]));
	}

	const short used = Game::Controls::useJoystick[selectedPlayer-1];
	interactables["joystick_toggle"]->getText()->setString(used == -1 ? "NO" :
			std::string("Joystick") + Game::to_string(used));

	return Action::DO_NOTHING;
}

void ControlsScreen::update() {
	Game::UI::Screen::update();
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto& text = interactables["p" + Game::to_string(i+1)];
		if (selectedPlayer == i + 1) 
			text->setColor(sf::Color::Yellow);
	}
	if (changingCtrlText != nullptr) 
		changingCtrlText->setFGColor(sf::Color::Red);
}

bool ControlsScreen::receiveEvent(const sf::Event& event) {
	if (changingCtrlText == nullptr) return false;

	auto get_control = [this] (Game::ShadedText *text) {
		using C = Game::Controls::Control;
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
			if (Game::Controls::useJoystick[selectedPlayer-1] < 0 ||
				short(event.joystickButton.joystickId) !=
					Game::Controls::useJoystick[selectedPlayer-1])
			{
				break;
			}
			Game::Controls::joystickBombKey[selectedPlayer-1] = event.joystickButton.button;
			std::stringstream ss;
			ss << "Button" << event.joystickButton.button;
			changingCtrlText->setString(ss.str());
			changingCtrlText->setFGColor(sf::Color::White);
			changingCtrlText = nullptr;
			break;
		}
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Key::Escape:
			_resyncCommandStrings();
			changingCtrlText = nullptr;
			return true;
		default:
			// Only meaningful if we're using the keyboard
			if (Game::Controls::useJoystick[selectedPlayer-1] >= 0) 
				break;

			Game::Controls::players[selectedPlayer-1][get_control(changingCtrlText)] = event.key.code;
			changingCtrlText->setString(Game::KeyUtils::keyToString(event.key.code));
			changingCtrlText->setFGColor(sf::Color::White);
			changingCtrlText = nullptr;
			break;
		}
		break;
	default:
		break;
	}

	return false;
}

void ControlsScreen::_resyncCommandStrings() {
	interactables["change_up"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::Controls::CTRL_UP]));
	interactables["change_down"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::Controls::CTRL_DOWN]));
	interactables["change_right"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::Controls::CTRL_RIGHT]));
	interactables["change_left"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::Controls::CTRL_LEFT]));
	interactables["change_bomb"]->getText()->setString(Game::Controls::useJoystick[selectedPlayer-1] >= 0
				? "Button" + Game::to_string(Game::Controls::joystickBombKey[selectedPlayer-1])
				: Game::KeyUtils::keyToString(
					Game::Controls::players[selectedPlayer-1][Game::Controls::CTRL_BOMB]));
}

Action ControlsScreen::_changeControl(const std::string& textKey) {
	if (Game::Controls::useJoystick[selectedPlayer-1] >= 0 && textKey != "change_bomb") {
		return Action::DO_NOTHING;
	}

	changingCtrlText = interactables[textKey]->getText();
	changingCtrlText->setString("_");

	return Action::DO_NOTHING;
}

Action ControlsScreen::_toggleJoystick() {
	// Retreive the list of connected joysticks
	std::vector<short> joysticks;
	joysticks.reserve(sf::Joystick::Count);
	
	// Index of the currently used joystick, or -1 if no joystick is in use
	short idx = -1;
	// Currently used joystick (-1 if none)
	short current = Game::Controls::useJoystick[selectedPlayer-1];

	for (auto i = 0; i < sf::Joystick::Count; ++i) {
		if (sf::Joystick::isConnected(i)) {
			joysticks.push_back(i);
			if (i == current) // found the currently used joystick
				idx = joysticks.size() - 1;
		}
	}

	if (current == -1)
		current = joysticks[0];
	else if (idx < static_cast<short>(joysticks.size()) - 1)
		current = joysticks[++idx];
	else
		current = -1;

	Game::Controls::useJoystick[selectedPlayer-1] = current;
	if (current == -1) {
		interactables["joystick_toggle"]->getText()->setString("NO");
		interactables["change_up"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Controls::CTRL_UP]));
		interactables["change_down"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Controls::CTRL_DOWN]));
		interactables["change_left"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Controls::CTRL_LEFT]));
		interactables["change_right"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Controls::CTRL_RIGHT]));
		interactables["change_bomb"]->getText()->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Controls::CTRL_BOMB]));
	} else {
		std::stringstream ss;
		ss << "Joystick" << current;
		interactables["joystick_toggle"]->getText()->setString(ss.str());
		interactables["change_up"]->getText()->setString("Up");
		interactables["change_down"]->getText()->setString("Down");
		interactables["change_left"]->getText()->setString("Left");
		interactables["change_right"]->getText()->setString("Right");
		ss.str("");
		ss << "Button" << Game::Controls::joystickBombKey[selectedPlayer-1];
		interactables["change_bomb"]->getText()->setString(ss.str());
	}

	return Action::DO_NOTHING;
}
