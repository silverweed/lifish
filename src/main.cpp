#include <iostream>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelRenderer.hpp"
#include "MovingEntity.hpp"

using Game::Direction;

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

	Game::Player *player1 = lr.getPlayer(1);

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
				case sf::Keyboard::Key::Add: {
					short n = level->getLevelNum() + 1;
					if (n > levelset.getLevelsNum())
						n = 1;
					lr.loadLevel(level = levelset.getLevel(n));
					break;
				}
				case sf::Keyboard::Key::Subtract: {
					short n = level->getLevelNum() - 1;
					if (n < 1)
						n = levelset.getLevelsNum();
					lr.loadLevel(level = levelset.getLevel(n));
					break;
				}
				default:
					break; 
				}
				break;
			default:
				break;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			player1->move(Direction::UP);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player1->move(Direction::LEFT);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			player1->move(Direction::DOWN);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			player1->move(Direction::RIGHT);
		else
			player1->stop();

		// FIXME: can we avoid clearing and drawing the background each frame?
		window.clear();
		lr.renderFrame(window);
		window.display();
	}
	return 0;
}
