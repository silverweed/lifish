#include <SFML/Window.hpp>
#include <iostream>

int main() {
	for (int i = 0; i < 8; ++i)
		if (sf::Joystick::isConnected(i))
			std::cerr << "Joystick " << i << " is connected." << std::endl;
}
