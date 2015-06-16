#include <SFML/Graphics.hpp>
#include "Level.hpp"

int main() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "Level test");

	Game::Level level("../assets/textures/level_test.png");
	level.draw(window);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Key::Escape:
					window.close();
					break;
				}
				break;
			}
		}
		window.clear();
		level.draw(window);
		window.display();
	}

}
