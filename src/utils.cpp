#include "utils.hpp"

short keyToNumber(sf::Keyboard::Key key) {
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

sf::Vector2i tile(const sf::Vector2f& pos) {
	return sf::Vector2i(
			(unsigned short)pos.x / Game::TILE_SIZE, 
			(unsigned short)pos.y / Game::TILE_SIZE);
}
