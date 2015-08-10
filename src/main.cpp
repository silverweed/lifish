/* Lifish
 * a game by Giacomo Parolini
 * inspired by Federico Filipponi's "BOOM"
 * 
 * Copyright (C) 2015 Giacomo Parolini
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelRenderer.hpp"
#include "SidePanel.hpp"
#include "MovingEntity.hpp"
#include "Controls.hpp"

using Game::TILE_SIZE;
using Game::MAIN_WINDOW_SHIFT;
using Game::LEVEL_WIDTH;
using Game::LEVEL_HEIGHT;

sf::Vector2f center(const sf::FloatRect& bounds);
Game::Level* advanceLevel(sf::RenderWindow& window, Game::LevelRenderer& lr, Game::SidePanel& panel);

int main(int argc, char **argv) {
	std::clog << "lifish v." << VERSION << " rev." << COMMIT << std::endl;	
	if (Game::init())
		std::clog << "Game successfully initialized. pwd = " << Game::pwd << std::endl;
	else {
		std::cerr << "Game failed to initialize!" << std::endl;
		return 1;
	}
	std::string levelSet = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	if (argc > 1)
		levelSet = std::string(argv[1]);

	// Create the rendering window
	sf::RenderWindow window(sf::VideoMode(
				Game::WINDOW_WIDTH, 
				Game::WINDOW_HEIGHT), "Level test");
	bool vsync = false;

	// Parse the level set
	Game::LevelSet levelset(levelSet);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	// Create the level renderer and side panel and attach the 1st level to them
	Game::Level *level = levelset.getLevel(1);
	level->printInfo();
	Game::LevelRenderer lr;
	lr.setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	lr.loadLevel(level);
	lr.renderFrame(window);
	Game::SidePanel panel(&lr);

	// Create fps text
	bool show_fps = false;
	sf::Clock fps_clock, fps_update_clock;
	Game::ShadedText fps_text(Game::getAsset("fonts", Game::Fonts::DEBUG_INFO),
			"-", sf::Vector2f(10, 10));
	fps_text.setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	fps_text.setStyle(sf::Text::Style::Bold);
	fps_text.setCharacterSize(20);
	Game::ShadedText vsync_text(Game::getAsset("fonts", Game::Fonts::DEBUG_INFO),
			"vsync off", sf::Vector2f(6 * TILE_SIZE, 10));
	vsync_text.setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	vsync_text.setCharacterSize(16);

	auto players = lr.getPlayers();
	bool gameOverTriggered = false;
	bool levelClearTriggered = false;
	sf::Clock levelClearClock;

	// Main game cycle
	while (window.isOpen()) {
		// If all players are dead, scroll down the GAME OVER text
		if (gameOverTriggered) {
			if (lr.isGameOverEnded()) {
				// TODO: end game
				window.close();
				break;
			}
		} else if (levelClearTriggered) {
			const auto time = levelClearClock.getElapsedTime().asSeconds();
			if (time >= 4) {
				level = advanceLevel(window, lr, panel);
				players = lr.getPlayers();
				levelClearTriggered = false;
				for (auto& player : players)
					player->setWinMode(false);
			} else if (time >= 1) {	
				for (auto& player : players)
					player->setWinMode(true);
			}
		}

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
#ifndef RELEASE
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
#endif
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

			if (players[i]->isAligned())
				players[i]->setDirection(dir[i]);
		}

		lr.checkHurryUp();

		if (lr.isExtraGame()) {
			lr.checkExtraGameEnd();
			lr.cycleLetters();
		}

		lr.checkLinesOfSight();

		lr.selectEnemyMoves();

		lr.makeBossesShoot();

		lr.checkBombExplosions();

		lr.checkExplosionHits();

		lr.detectCollisions();

		unsigned short dead_players = 0;
		for (unsigned short i = 0; i < 2; ++i) {
			if (players[i] == nullptr) {
				++dead_players;
				continue;
			}
			if (players[i]->isAligned()) {
				players[i]->prevAlign = Game::tile(players[i]->getPosition());
				if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::BOMB]))
					lr.dropBomb(i);
			}

			if (players[i]->isHurt()) {
				players[i]->prepareHurtAnimation();
				if (!players[i]->playHurtAnimation())
					players[i]->setHurt(false);
			} else if (players[i]->isDying()) {
				players[i]->prepareDeathAnimation();
				if (!players[i]->playDeathAnimation()) {
					if (players[i]->getRemainingLives() <= 0) {
						lr.removePlayer(i + 1);
						players[i] = nullptr;
					} else {
						players[i]->resurrect();
						players[i]->giveShield(Game::RESURRECT_SHIELD_TIME);
					}
				}
			} else if (players[i]->getDirection() == Game::Direction::NONE) {
				players[i]->stop();
			} else {
				players[i]->move();
			}
		}
		
		// Check game over / win
		if (dead_players == Game::MAX_PLAYERS && !gameOverTriggered) {
			lr.triggerGameOver();
			gameOverTriggered = true;
		} else if (!levelClearTriggered && lr.isLevelClear()) {
			levelClearTriggered = true;
			levelClearClock.restart();
		}

		lr.moveBullets();

		// Actually move enemies
		lr.applyEnemyMoves();

		float cur_time = fps_clock.restart().asSeconds();
		if (show_fps && fps_update_clock.getElapsedTime().asSeconds() >= 1) {
			int fps = (int)(1.f / cur_time);
			std::stringstream ss;
			ss << fps << " fps"; 
			fps_text.setString(ss.str());
			fps_update_clock.restart();
		}

		window.clear();
		lr.renderFrame(window);
		panel.draw(window);
		if (show_fps) {
			fps_text.draw(window);
			vsync_text.draw(window);
		}
		window.display();
	}
	return 0;
}

Game::Level* advanceLevel(sf::RenderWindow& window, Game::LevelRenderer& lr, Game::SidePanel& panel) {
	// Display the time bonus on screen
	static sf::Font font;
	static bool loaded = false;
	if (!loaded) {
		const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
		loaded = font.loadFromFile(fontname);
	}
	sf::Text text("LEVEL COMPLETE!", font, 13);
	text.setPosition(center(text.getGlobalBounds()));

	window.clear();
	window.draw(text);

	const auto timeBonus = lr.getTimeLeft();
	if (timeBonus > 0) {
		// TODO: draw
	}
	
	panel.draw(window);
	window.display();

	std::this_thread::sleep_for(std::chrono::seconds(3));

	// TODO: assign points

	const auto level = lr.getLevel();
	const auto levelSet = level->getLevelSet();
	short lvnum = level->getLevelNum();

	if (lvnum == levelSet->getLevelsNum()) {
		// TODO: WIN!
		return nullptr;
	} else {
		lr.loadLevel(levelSet->getLevel(++lvnum));
	}

	std::stringstream ss;
	ss << "LEVEL " << lvnum;
	text.setString(ss.str());
	text.setPosition(center(text.getGlobalBounds()));

	window.clear();
	window.draw(text);

	text.setString("GET READY!");
	const auto bounds = text.getGlobalBounds();
	text.setPosition(center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

	window.draw(text);
	panel.draw(window);
	window.display();

	std::this_thread::sleep_for(std::chrono::seconds(3));

	return const_cast<Game::Level*>(lr.getLevel());
}

/** Given the bounding box of something, returns the coordinates
 *  which center that thing relatively to the main window.
 */
sf::Vector2f center(const sf::FloatRect& bounds) {
	return sf::Vector2f(MAIN_WINDOW_SHIFT + (LEVEL_WIDTH * TILE_SIZE - bounds.width) / 2.,
			(LEVEL_HEIGHT * TILE_SIZE - bounds.height) / 2.);
}
