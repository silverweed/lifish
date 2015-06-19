#pragma GCC diagnostic ignored "-Wswitch"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"

int main() {
	if (Game::init()) {
		std::clog << "Game successfully initialized. Version = " << VERSION << "; pwd = " << Game::pwd << std::endl;
	} else {
		std::cerr << "Game failed to initialize!" << std::endl;
		return 1;
	}
	sf::RenderWindow window(sf::VideoMode(640, 480), "Level test");

	Game::LevelSet levelset(std::string(Game::pwd) + std::string("/levels.json"));
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	Game::Level *level = levelset.getLevel(2);
	level->printInfo();
	level->draw(window);

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
		level->draw(window);
		window.display();
	}
	return 0;
}
