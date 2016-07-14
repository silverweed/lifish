#include <SFML/Window.hpp>
#include <iostream>

int main() {
	sf::Window win(sf::VideoMode(10, 10), "up");

	while (win.isOpen()) {
		sf::Event event;
		while (win.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				win.close();
		}

		std::cerr<<"UP is pressed : " << sf::Keyboard::isKeyPressed(sf::Keyboard::Up) << std::endl;
		std::cerr<<"Down is pressed : " << sf::Keyboard::isKeyPressed(sf::Keyboard::Down) << std::endl;
		std::cerr<<"Left is pressed : " << sf::Keyboard::isKeyPressed(sf::Keyboard::Left) << std::endl;
		std::cerr<<"Right is pressed : " << sf::Keyboard::isKeyPressed(sf::Keyboard::Right) << std::endl;
	}
}
