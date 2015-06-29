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
	std::string levelSet = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	if (argc > 1)
		levelSet = std::string(argv[1]);

	sf::RenderWindow window(sf::VideoMode(800, 600), "Level test");

	Game::LevelSet levelset(levelSet);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	Game::Level *level = levelset.getLevel(1);
	level->printInfo();
	Game::LevelRenderer lr;
	lr.loadLevel(level);
	lr.renderFrame(window);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (sf::Keyboard::Key key = event.key.code) {
				case sf::Keyboard::Key::Escape:
					window.close();
					break;
				case sf::Keyboard::Key::Right: {
					short n = level->getLevelNum() + 1;
					if (n > levelset.getLevelsNum())
						n = 1;
					lr.loadLevel(level = levelset.getLevel(n));
					break;
				}
				case sf::Keyboard::Key::Left: {
					short n = level->getLevelNum() - 1;
					if (n < 1)
						n = levelset.getLevelsNum();
					lr.loadLevel(level = levelset.getLevel(n));
					break;
				}
				default: {
					short n = keyToNumber(key);
					if (n >= 0) {
						level = levelset.getLevel(n);
						if (level != nullptr)
							lr.loadLevel(level);
					}
				}
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
