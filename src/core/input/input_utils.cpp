#include "input_utils.hpp"
#include <iostream>

//constexpr unsigned XBOX = 11;
constexpr unsigned PS3 = 12;

#ifdef __APPLE__
std::array<bool, sf::Keyboard::KeyCount> lif::kb::keyPressed;

bool lif::kb::isKeyPressed(sf::Keyboard::Key key) {
	using lif::kb::keyPressed;

	const int idx = static_cast<int>(key);
	return key >= 0 && key < static_cast<int>(keyPressed.size()) ? keyPressed[idx] : false;
}

void lif::kb::checkKeyPressed(sf::Event event) {
	using lif::kb::keyPressed;

	switch (event.type) {
	case sf::Event::KeyPressed:
	case sf::Event::KeyReleased:
		{
			const auto key = event.key.code;
			const int idx = static_cast<int>(key);
			if (key >= 0 && key < static_cast<int>(keyPressed.size()))
				keyPressed[idx] = event.type == sf::Event::KeyPressed;
			break;
		}
	default:
		break;
	}
}
#endif

short lif::joystick::getButton(lif::joystick::ButtonType type, unsigned id) {
	if (!sf::Joystick::isConnected(id)) return -1;
	// Heuristic approach to "known" joysticks: we distinguish them by their
	// button count and hope they're really that type of joystick.
	// We could use vendor / product ID, but maybe this method is more resilient
	// to slightly different models (or maybe not).
	// Xbox 360 controller has 11 buttons
	// PS3 controller has 12 buttons
	// Note: for now, we always fallback to Xbox if controller is not PS3.
	const auto bc = sf::Joystick::getButtonCount(id);
	switch (type) {
		using BT = lif::joystick::ButtonType;
	case BT::START:
		return bc == PS3 ? 9 : 7;
	case BT::SELECT:
		return bc == PS3 ? 8 : 6;
	case BT::BTN_DOWN:
		return bc == PS3 ? 2 : 0;
	case BT::BTN_RIGHT:
		return 1;
	default:
		break;
	}
	return -1;
}

bool lif::joystick::isButton(lif::joystick::ButtonType type, unsigned id, short btn) {
	const short b = lif::joystick::getButton(type, id);
	return b >= 0 && btn == b;
}

short lif::kb::keyToNumber(sf::Keyboard::Key key) {
	switch (key) {
		using K = sf::Keyboard::Key;
	case K::Num0: case K::Numpad0: return 0;
	case K::Num1: case K::Numpad1: return 1;
	case K::Num2: case K::Numpad2: return 2;
	case K::Num3: case K::Numpad3: return 3;
	case K::Num4: case K::Numpad4: return 4;
	case K::Num5: case K::Numpad5: return 5;
	case K::Num6: case K::Numpad6: return 6;
	case K::Num7: case K::Numpad7: return 7;
	case K::Num8: case K::Numpad8: return 8;
	case K::Num9: case K::Numpad9: return 9;
	default: return -1;
	}
}

std::string lif::kb::keyToString(sf::Keyboard::Key key) {
	switch (key) {
		using K = sf::Keyboard::Key;
	case K::A: return "A";
	case K::B: return "B";
	case K::C: return "C";
	case K::D: return "D";
	case K::E: return "E";
	case K::F: return "F";
	case K::G: return "G";
	case K::H: return "H";
	case K::I: return "I";
	case K::J: return "J";
	case K::K: return "K";
	case K::L: return "L";
	case K::M: return "M";
	case K::N: return "N";
	case K::O: return "O";
	case K::P: return "P";
	case K::Q: return "Q";
	case K::R: return "R";
	case K::S: return "S";
	case K::T: return "T";
	case K::U: return "U";
	case K::V: return "V";
	case K::W: return "W";
	case K::X: return "X";
	case K::Y: return "Y";
	case K::Z: return "Z";
	case K::Num0: return "Num0";
	case K::Num1: return "Num1";
	case K::Num2: return "Num2";
	case K::Num3: return "Num3";
	case K::Num4: return "Num4";
	case K::Num5: return "Num5";
	case K::Num6: return "Num6";
	case K::Num7: return "Num7";
	case K::Num8: return "Num8";
	case K::Num9: return "Num9";
	case K::Escape: return "Escape";
	case K::LControl: return "LControl";
	case K::LShift: return "LShift";
	case K::LAlt: return "LAlt";
	case K::LSystem: return "LSystem";
	case K::RControl: return "RControl";
	case K::RShift: return "RShift";
	case K::RAlt: return "RAlt";
	case K::RSystem: return "RSystem";
	case K::Menu: return "Menu";
	case K::LBracket: return "LBracket";
	case K::RBracket: return "RBracket";
	case K::SemiColon: return "SemiColon";
	case K::Comma: return "Comma";
	case K::Period: return "Period";
	case K::Quote: return "Quote";
	case K::Slash: return "Slash";
	case K::BackSlash: return "BackSlash";
	case K::Tilde: return "Tilde";
	case K::Equal: return "Equal";
	case K::Dash: return "Dash";
	case K::Space: return "Space";
	case K::Return: return "Return";
	case K::BackSpace: return "BackSpace";
	case K::Tab: return "Tab";
	case K::PageUp: return "PageUp";
	case K::PageDown: return "PageDown";
	case K::End: return "End";
	case K::Home: return "Home";
	case K::Insert: return "Insert";
	case K::Delete: return "Delete";
	case K::Add: return "Add";
	case K::Subtract: return "Subtract";
	case K::Multiply: return "Multiply";
	case K::Divide: return "Divide";
	case K::Left: return "Left";
	case K::Right: return "Right";
	case K::Up: return "Up";
	case K::Down: return "Down";
	case K::Numpad0: return "Numpad0";
	case K::Numpad1: return "Numpad1";
	case K::Numpad2: return "Numpad2";
	case K::Numpad3: return "Numpad3";
	case K::Numpad4: return "Numpad4";
	case K::Numpad5: return "Numpad5";
	case K::Numpad6: return "Numpad6";
	case K::Numpad7: return "Numpad7";
	case K::Numpad8: return "Numpad8";
	case K::Numpad9: return "Numpad9";
	case K::F1: return "F1";
	case K::F2: return "F2";
	case K::F3: return "F3";
	case K::F4: return "F4";
	case K::F5: return "F5";
	case K::F6: return "F6";
	case K::F7: return "F7";
	case K::F8: return "F8";
	case K::F9: return "F9";
	case K::F10: return "F10";
	case K::F11: return "F11";
	case K::F12: return "F12";
	case K::F13: return "F13";
	case K::F14: return "F14";
	case K::F15: return "F15";
	case K::Pause: return "Pause";
	default: return "Unknown";
	}
}

char lif::kb::keyToAlnum(sf::Keyboard::Key key) {
#define ALPHA(x, X)  \
	(IS_KEY_PRESSED(sf::Keyboard::LShift) || IS_KEY_PRESSED(sf::Keyboard::RShift) \
		? X : x)

	switch (key) {
		using K = sf::Keyboard::Key;
	case K::A: return ALPHA('a', 'A');
	case K::B: return ALPHA('b', 'B');
	case K::C: return ALPHA('c', 'C');
	case K::D: return ALPHA('d', 'D');
	case K::E: return ALPHA('e', 'E');
	case K::F: return ALPHA('f', 'F');
	case K::G: return ALPHA('g', 'G');
	case K::H: return ALPHA('h', 'H');
	case K::I: return ALPHA('i', 'I');
	case K::J: return ALPHA('j', 'J');
	case K::K: return ALPHA('k', 'K');
	case K::L: return ALPHA('l', 'L');
	case K::M: return ALPHA('m', 'M');
	case K::N: return ALPHA('n', 'N');
	case K::O: return ALPHA('o', 'O');
	case K::P: return ALPHA('p', 'P');
	case K::Q: return ALPHA('q', 'Q');
	case K::R: return ALPHA('r', 'R');
	case K::S: return ALPHA('s', 'S');
	case K::T: return ALPHA('t', 'T');
	case K::U: return ALPHA('u', 'U');
	case K::V: return ALPHA('v', 'V');
	case K::W: return ALPHA('w', 'W');
	case K::X: return ALPHA('x', 'X');
	case K::Y: return ALPHA('y', 'Y');
	case K::Z: return ALPHA('z', 'Z');
	case K::Num0: case K::Numpad0: return '0';
	case K::Num1: case K::Numpad1: return '1';
	case K::Num2: case K::Numpad2: return '2';
	case K::Num3: case K::Numpad3: return '3';
	case K::Num4: case K::Numpad4: return '4';
	case K::Num5: case K::Numpad5: return '5';
	case K::Num6: case K::Numpad6: return '6';
	case K::Num7: case K::Numpad7: return '7';
	case K::Num8: case K::Numpad8: return '8';
	case K::Num9: case K::Numpad9: return '9';
	case K::Space: return ' ';
	default: return -1;
	}
#undef ALPHA
}
