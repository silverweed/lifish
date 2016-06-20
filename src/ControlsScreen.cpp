#include "ControlsScreen.hpp"
#include "Controls.hpp"
#include "Game.hpp"
#include "Options.hpp"
#include "utils.hpp"

using Game::ControlsScreen;
using Game::Control;

ControlsScreen::ControlsScreen() : Screen () {
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

	const auto size = 24;
	const float ipadx = 25,
		    ipady = 15;
	auto text = new Game::ShadedText(font, "P1", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	text->setFGColor(sf::Color::Red);
	texts["controls::p1"] = text;

	auto bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, " / ", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "P2", sf::Vector2f(bounds.left + bounds.width, ipady));
	text->setCharacterSize(size);
	texts["controls::p2"] = text;

	float y = bounds.top + 3 * bounds.height,
	      x = 180;
	const auto small_size = 20;
	text = new Game::ShadedText(font, "UP: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Game::CTRL_UP]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_up"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "DOWN: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Game::CTRL_DOWN]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_down"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "LEFT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Game::CTRL_LEFT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_left"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "RIGHT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Game::CTRL_RIGHT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_right"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "BOMB: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::Controls::players[selectedPlayer-1][Game::CTRL_BOMB]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_bomb"] = text;

	y += 3 * bounds.height;
	text = new Game::ShadedText(font, "Use Joystick?", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "NO", sf::Vector2f(bounds.left + bounds.width + 20, y));
	text->setCharacterSize(small_size);
	texts["controls::joystick_toggle"] = text;

	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	text = new Game::ShadedText(font, "OK", sf::Vector2f(0, 0));
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(Game::center(bounds, win_bounds).x, win_bounds.height - 2 * bounds.height));
	texts["exit"] = text;
}

void ControlsScreen::selectPlayer(unsigned short id) {
	if (selectedPlayer == id) return;
	texts["controls::p" + Game::to_string(selectedPlayer)]->setFGColor(sf::Color::White);
	selectedPlayer = id;
	
	const std::array<std::pair<const char*, Game::Control>, 5> pairs = {{
		std::make_pair("up", Game::CTRL_UP),
		std::make_pair("down", Game::CTRL_DOWN),
		std::make_pair("left", Game::CTRL_LEFT),
		std::make_pair("right", Game::CTRL_RIGHT),
		std::make_pair("bomb", Game::CTRL_BOMB)
	}};

	for (auto it = pairs.begin(); it != pairs.end(); ++it) {
		std::stringstream ss;
		ss << "controls::change_" << it->first;
		texts[ss.str()]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][it->second]));
	}

	const short used = Game::options.useJoystick[selectedPlayer-1];
	texts["controls::joystick_toggle"]->setString(used == -1 ? "NO" :
			std::string("Joystick") + Game::to_string(used));
}

void ControlsScreen::_highlightSelectedPlayer() {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto text = texts["controls::p" + Game::to_string(i+1)];
		if (selectedPlayer == i + 1) 
			text->setFGColor(sf::Color::Yellow);
	}
}

void ControlsScreen::triggerMouseOver(const sf::Vector2f& mousePos) {
	Game::Screen::triggerMouseOver(mousePos);
	_highlightSelectedPlayer();
}

void ControlsScreen::changeControl(sf::RenderWindow& window, const std::string& textKey) {
	Control control = Game::Controls::fromString(textKey.substr(17));
	if (Game::options.useJoystick[selectedPlayer-1] >= 0) {
		if (control == Game::CTRL_BOMB) {
			_changeJoystickBomb(window);
			return;
		} else {
			return;
		}
	}

	auto text = texts[textKey];
	text->setString("_");
	text->setFGColor(sf::Color::Red);

	window.draw(*text);
	window.display();

	sf::Keyboard::Key original_command = Game::Controls::players[selectedPlayer-1][control];

	// Wait for input
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				return;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Key::Escape:
					text->setString(Game::KeyUtils::keyToString(original_command));
					return;
				default:
					Game::Controls::players[selectedPlayer-1][control] = event.key.code;
					text->setString(Game::KeyUtils::keyToString(event.key.code));
					// TODO: invalidate other commands with the same key
					return;
				}
			default:
				break;
			}
		}

		window.clear();
		window.draw(*this);
		window.display();
	}
}

void ControlsScreen::_changeJoystickBomb(sf::RenderWindow& window) {
	auto text = texts["controls::change_bomb"];
	text->setString("_");
	text->setFGColor(sf::Color::Red);

	window.draw(*text);
	window.display();

	unsigned int original_command = Game::Controls::joystickBombKey[selectedPlayer-1];

	// Wait for button input
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				return;
			case sf::Event::JoystickButtonPressed:
				{
					if (short(event.joystickButton.joystickId) != Game::options.useJoystick[selectedPlayer-1])
						break;
					Game::Controls::joystickBombKey[selectedPlayer-1] = event.joystickButton.button;
					std::stringstream ss;
					ss << "Button" << event.joystickButton.button;
					text->setString(ss.str());
					// TODO: invalidate other commands with the same key
					return;
				}
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) {
					std::stringstream ss;
					ss << "Button" << original_command;
					text->setString(ss.str());
					return;
				}
				break;
			default:
				break;
			}
		}

		window.clear();
		window.draw(*this);
		window.display();
	}
}

void ControlsScreen::toggleJoystick() {
	// Retreive the list of connected joysticks
	std::vector<short> joysticks;
	joysticks.reserve(sf::Joystick::Count);
	
	// Index of the currently used joystick, or -1 if no joystick is in use
	short idx = -1;
	// Currently used joystick (-1 if none)
	short current = Game::options.useJoystick[selectedPlayer-1];

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

	Game::options.useJoystick[selectedPlayer-1] = current;
	if (current == -1) {
		texts["controls::joystick_toggle"]->setString("NO");
		texts["controls::change_up"]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::CTRL_UP]));
		texts["controls::change_down"]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::CTRL_DOWN]));
		texts["controls::change_left"]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::CTRL_LEFT]));
		texts["controls::change_right"]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::CTRL_RIGHT]));
		texts["controls::change_bomb"]->setString(Game::KeyUtils::keyToString(
				Game::Controls::players[selectedPlayer-1][Game::CTRL_BOMB]));
	} else {
		std::stringstream ss;
		ss << "Joystick" << current;
		texts["controls::joystick_toggle"]->setString(ss.str());
		texts["controls::change_up"]->setString("UP");
		texts["controls::change_down"]->setString("DOWN");
		texts["controls::change_left"]->setString("LEFT");
		texts["controls::change_right"]->setString("RIGHT");
		ss.str("");
		ss << "Button" << Game::Controls::joystickBombKey[selectedPlayer-1];
		texts["controls::change_bomb"]->setString(ss.str());
	}
}
