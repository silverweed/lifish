#include <iostream>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelRenderer.hpp"

int main(int argc, char **argv) {
	std::clog << "lifish v." << VERSION << " rev." << COMMIT << std::endl;	
	if (Game::init()) {
		std::clog << "Game successfully initialized. pwd = " << Game::pwd << std::endl;

	} else {
		std::cerr << "Game failed to initialize!" << std::endl;
		return 1;
	}
	sf::RenderWindow window(sf::VideoMode(800, 600), "Level test");

	Game::LevelSet levelset(std::string(Game::pwd) + std::string("/levels.json"));
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	Game::Level *level = levelset.getLevel(1);
	level->printInfo();
	Game::LevelRenderer lr(level);
	lr.loadEntities();
	lr.renderFrame(window);

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
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		window.clear();
		lr.renderFrame(window);
		window.display();
	}
	return 0;
}
