#include "SaveScreen.hpp"
#include "GameCache.hpp"
#include "Interactable.hpp"
#include "ShadedText.hpp"
#include "game.hpp"
#include "language.hpp"
#include "input_utils.hpp"
#include "utils.hpp"

using lif::ui::SaveScreen;

static const sf::Time CURSOR_BLINK_TIME = sf::seconds(1);

SaveScreen::SaveScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::DynamicScreen(window, sz)
{
	build();
}

void SaveScreen::build() {
	buffer.fill('\0');

	name = SCREEN_NAME;
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * [Errtext]
	 * Insert save file name
	 * [buffer]
	 * --
	 * [Save]
	 * [Back]
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, size.x, size.y);
	const auto size = 24;

	auto text = new lif::ShadedText(font, lif::getLocalized("insert_save_file"));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	text->setPosition(lif::center(text->getGlobalBounds(), win_bounds) - sf::Vector2f(0, 50));
	nonInteractables.emplace_back(text);

	errText = new lif::ShadedText(font, "Dummy");
	errText->setCharacterSize(20);
	errText->setFGColor(sf::Color(255, 38, 38));
	//errText->setBGColor(sf::Color(180, 180, 180));
	errText->setShadowSpacing(2, 2);
	auto bounds = errText->getGlobalBounds();
	errText->setPosition(sf::Vector2f(text->getPosition().x, text->getPosition().y - bounds.height * 1.5));
	errText->setString("");
	nonInteractables.emplace_back(errText);

	// BufferText
	bufferText = new lif::ShadedText(font, std::string(MAX_SAVE_FILE_NAME_LEN, 'D'));
	bufferText->setCharacterSize(size);
	bounds = bufferText->getGlobalBounds();
	bufferText->setPosition(lif::center(bounds, win_bounds));
	nonInteractables.emplace_back(bufferText);

	/// Cursor
	bufferText->setString("D");
	bounds = bufferText->getGlobalBounds();
	charBounds = sf::Vector2f(bounds.width, bounds.height);
	bufferText->setString("");
	cursor = new sf::RectangleShape(sf::Vector2f(charBounds.x, 6));
	_updateCursorPosition();
	nonInteractables.emplace_back(cursor);

	// Confirm
	text = new lif::ShadedText(font, lif::getLocalized("do_save"));
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 6 * bounds.height));
	interactables["save"] = std::make_unique<lif::ui::Interactable>(text);
	callbacks["save"] = [this] () {
		if (bufIdx > 0)
			return lif::ui::Action::SAVE_GAME;
		errText->setString("Invalid file name");
		return lif::ui::Action::DO_NOTHING;
	};

	// Back
	text = new lif::ShadedText(font, lif::getLocalized("back"));
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<lif::ui::Interactable>(text);

	// Transitions
	transitions.add("save", std::make_pair(lif::Direction::DOWN, "back"));
}

void SaveScreen::update() {
	Screen::update();

	// Blink cursor
	if (cursorClock.getElapsedTime() > CURSOR_BLINK_TIME) {
		cursorVisible = !cursorVisible;
		cursor->setFillColor(cursorVisible ? sf::Color::White : sf::Color::Transparent);
		cursorClock.restart();
	}
}

void SaveScreen::onLoad() {
	Screen::onLoad();

	buffer.fill('\0');
	bufIdx = 0;
	bufferText->setString("");
}

bool SaveScreen::handleEvent(sf::Window& win, sf::Event event) {
	if (event.type != sf::Event::KeyPressed)
		return Screen::handleEvent(win, event);

	switch (event.key.code) {
	case sf::Keyboard::BackSpace:
		// Cancel
		if (IS_KEY_PRESSED(sf::Keyboard::LControl)) {
			buffer.fill('\0');
			bufIdx = 0;
		} else if (bufIdx > 0) {
			buffer[--bufIdx] = '\0';
		}
		bufferText->setString(std::string(buffer.data(), bufIdx));
		_updateCursorPosition();
		return true;
	case sf::Keyboard::Return:
		// Confirm
		if (bufIdx > 0)
			_triggerAction(lif::ui::Action::SAVE_GAME);
		else
			errText->setString(lif::getLocalized("invalid_file_name"));
		return true;
	case sf::Keyboard::Up:
	case sf::Keyboard::Left:
	case sf::Keyboard::Down:
	case sf::Keyboard::Right:
		// These are used for menu navigation!
		return Screen::handleEvent(win, event);
	default:
		break;
	}

	const char ch = lif::kb::keyToAlnum(event.key.code);
	if (ch == (char)-1)
		return false;

	errText->setString("");

	if (bufIdx < buffer.size()) {
		buffer[bufIdx++] = ch;
		bufferText->setString(std::string(buffer.data(), bufIdx));
		_updateCursorPosition();
	}

	return true;
}

void SaveScreen::_updateCursorPosition() {
	const auto txtbounds = bufferText->getGlobalBounds();
	cursor->setPosition(bufferText->getPosition() + sf::Vector2f(txtbounds.width, charBounds.y));
	cursorVisible = true;
	cursor->setFillColor(sf::Color::White);
	cursorClock.restart();
}

std::string SaveScreen::getPrompt() const {
	return std::string(buffer.data(), bufIdx);
}
