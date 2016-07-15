#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "Options.hpp"
#include "utils.hpp"

using namespace Game::UI;

int main(int argc, char **argv) {
	sf::RenderWindow window(sf::VideoMode(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), "screen test");
	window.setVerticalSyncEnabled(true);
	
	Game::init();
	Game::options.showFPS = true;
	Screen screen(argc > 1 ? argv[1] : "home.json", sf::Vector2u(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::F:
					Game::options.showFPS = !Game::options.showFPS;
					break;
				default:
					break;
				}
			default:
				break;
			}
		}

		window.clear();
		window.draw(screen);
		Game::maybeShowFPS(window);
		window.display();
	}
}
