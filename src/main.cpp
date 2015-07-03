#include <iostream>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelRenderer.hpp"
#include "MovingEntity.hpp"
#include "Controls.hpp"

using Game::TILE_SIZE;

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

	auto players = lr.getPlayers();

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
				case sf::Keyboard::Key::Add:
					{
						short n = level->getLevelNum() + 1;
						if (n > levelset.getLevelsNum())
							n = 1;
						lr.loadLevel(level = levelset.getLevel(n));
						players = lr.getPlayers();
						break;
					}
				case sf::Keyboard::Key::Subtract: 
					{
						short n = level->getLevelNum() - 1;
						if (n < 1)
							n = levelset.getLevelsNum();
						lr.loadLevel(level = levelset.getLevel(n));
						players = lr.getPlayers();
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

		Game::Direction dir[] = { Game::Direction::NONE, Game::Direction::NONE };
		for (unsigned int i = 0; i < 2; ++i) {
			if (players[i] == nullptr) continue;

			if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::UP]))
				dir[i] = Game::Direction::UP;
			else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::LEFT]))
				dir[i] = Game::Direction::LEFT;
			else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::DOWN]))
				dir[i] = Game::Direction::DOWN;
			else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::RIGHT]))
				dir[i] = Game::Direction::RIGHT;

			if (players[i]->isAligned()) {
				players[i]->setDirection(dir[i]);
			}
		}

		// Collisions detection
		lr.detectCollisions();

		for (unsigned int i = 0; i < 2; ++i) {
			if (players[i]->getDirection() == Game::Direction::NONE)
				players[i]->stop();
			else
				players[i]->move();
		}

		// XXX: can we avoid clearing and drawing the background each frame?
		window.clear();
		lr.renderFrame(window);
		window.display();
	}
	return 0;
}
