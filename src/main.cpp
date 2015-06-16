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
			}
		}
		level.draw(window);
		window.display();
	}

}
