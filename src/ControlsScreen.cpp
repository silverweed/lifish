#include "ControlsScreen.hpp"
#include "Controls.hpp"
#include "Game.hpp"
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
			Game::KeyUtils::keyToString(Game::playerControls[selectedPlayer-1][Control::UP]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_up"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "DOWN: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::playerControls[selectedPlayer-1][Control::DOWN]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_down"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "LEFT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::playerControls[selectedPlayer-1][Control::LEFT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_left"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "RIGHT: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::playerControls[selectedPlayer-1][Control::RIGHT]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_right"] = text;

	y += 2 * bounds.height;
	text = new Game::ShadedText(font, "BOMB: ", sf::Vector2f(ipadx, y));
	text->setCharacterSize(small_size);
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font,
			Game::KeyUtils::keyToString(Game::playerControls[selectedPlayer-1][Control::BOMB]),
			sf::Vector2f(x, y));
	text->setCharacterSize(small_size);
	texts["controls::change_bomb"] = text;

	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	text = new Game::ShadedText(font, "Exit", sf::Vector2f(0, 0));
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
		std::make_pair("up", Control::UP),
		std::make_pair("down", Control::DOWN),
		std::make_pair("left", Control::LEFT),
		std::make_pair("right", Control::RIGHT),
		std::make_pair("bomb", Control::BOMB)
	}};

	for (auto it = pairs.begin(); it != pairs.end(); ++it) {
		std::stringstream ss;
		ss << "controls::change_" << it->first;
		texts[ss.str()]->setString(Game::KeyUtils::keyToString(
				Game::playerControls[selectedPlayer-1][it->second]));
	}
}

void ControlsScreen::_highlightSelectedPlayer() {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto text = texts["controls::p" + Game::to_string(i+1)];
		if (selectedPlayer == i + 1) 
			text->setFGColor(sf::Color::Yellow);
	}
}

void ControlsScreen::draw(sf::RenderTarget& window) {
	Game::Screen::draw(window);
	_highlightSelectedPlayer();
}

void ControlsScreen::triggerMouseOver(const sf::Vector2f& mousePos) {
	Game::Screen::triggerMouseOver(mousePos);
	_highlightSelectedPlayer();
}

void ControlsScreen::changeControl(sf::RenderWindow& window, const std::string& textKey) {
	auto text = texts[textKey];
	text->setString("_");
	text->setFGColor(sf::Color::Red);

	window.draw(*text);
	window.display();

	Control control = Game::controlFromString(textKey.substr(17));
	sf::Keyboard::Key original_command = Game::playerControls[selectedPlayer-1][control];

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
					Game::playerControls[selectedPlayer-1][control] = event.key.code;
					text->setString(Game::KeyUtils::keyToString(event.key.code));
					// TODO: invalidate other commands with the same key
					return;
				}
			default:
				break;
			}
		}

		window.clear();
		draw(window);
		window.display();
	}
}
