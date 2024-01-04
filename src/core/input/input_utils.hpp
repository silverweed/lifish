#pragma once

#include <string>
#include <array>
#include <SFML/Window.hpp>

namespace lif {

namespace kb {

#ifdef __APPLE__
	extern std::array<bool, sf::Keyboard::KeyCount> keyPressed;	

	/** Polyfill for SFML's isKeyPressed (which doesn't work on OSX) */
	bool isKeyPressed(sf::Keyboard::Key key);

	/** If event is a KeyPressed/KeyReleased event, updates keyPressed status. */
	void checkKeyPressed(sf::Event event);

#	define IS_KEY_PRESSED lif::kb::isKeyPressed
#else
#	define IS_KEY_PRESSED sf::Keyboard::isKeyPressed
#endif

	/** Converts a key like Num0/Numpad0 to the respective number */
	short keyToNumber(sf::Keyboard::Key key);

	/** @return The display name of the given key */
	std::string keyToString(sf::Keyboard::Key key);

	/** @return The alphanumeric char corresponding to the given key, or -1 if non-alphanumeric. */
	char keyToAlnum(sf::Keyboard::Key key);
}

namespace joystick {
	enum class ButtonType {
		START,
		SELECT,
		BTN_DOWN,
		BTN_RIGHT
	};
	short getButton(ButtonType type, unsigned joystickId);
	bool isButton(ButtonType type, unsigned joystickId, short btn);
}

}
