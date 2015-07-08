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
	bool vsync = false;

	Game::LevelSet levelset(levelSet);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	Game::Level *level = levelset.getLevel(1);
	level->printInfo();
	Game::LevelRenderer lr;
	lr.setOrigin(-Game::MAIN_WINDOW_SHIFT);
	lr.loadLevel(level);
	lr.renderFrame(window);

	bool show_fps = false;
	sf::Clock fps_clock, fps_update_clock;
	Game::ShadedText fps_text(Game::getAsset("fonts", "pf_tempesta_seven_condensed.ttf"),
			"-", sf::Vector2f(10, 10));
	fps_text.setOrigin(-Game::MAIN_WINDOW_SHIFT);
	fps_text.setStyle(sf::Text::Style::Bold);
	fps_text.setCharacterSize(20);
	Game::ShadedText vsync_text(Game::getAsset("fonts", "pf_tempesta_seven_condensed.ttf"),
			vsync ? "vsync on" : "vsync off", sf::Vector2f(6 * TILE_SIZE, 10));
	vsync_text.setOrigin(-Game::MAIN_WINDOW_SHIFT);
	vsync_text.setCharacterSize(16);

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
				case sf::Keyboard::Key::F:
					show_fps = !show_fps;
					break;
				case sf::Keyboard::Key::V:
					vsync = !vsync;
					vsync_text.setString(vsync ? "vsync on" : "vsync off");
					window.setVerticalSyncEnabled(vsync);
					break;
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

		// Choose enemy movements
		lr.selectEnemyMoves();

		// Collisions detection
		lr.detectCollisions();

		for (unsigned int i = 0; i < 2; ++i) {
			if (players[i]->isAligned()) {
				players[i]->prevAlign = Game::tile(players[i]->getPosition());
				if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::BOMB]))
					lr.dropBomb(i);
			}

			if (players[i]->getDirection() == Game::Direction::NONE)
				players[i]->stop();
			else
				players[i]->move();
		}

		// Actually move enemies
		lr.applyEnemyMoves();

		float cur_time = fps_clock.restart().asSeconds();
		if (fps_update_clock.getElapsedTime().asSeconds() >= 1) {
			int fps = (int)(1.f / cur_time);
			std::stringstream ss;
			ss << fps << " fps"; 
			fps_text.setString(ss.str());
			fps_update_clock.restart();
		}

		// XXX: can we avoid clearing and drawing the background each frame?
		window.clear();
		lr.renderFrame(window);
		if (show_fps) {
			fps_text.draw(window);
			vsync_text.draw(window);
		}
		window.display();
	}
	return 0;
}
