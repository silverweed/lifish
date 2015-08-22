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
#include <cstdlib>
#if defined(SFML_SYSTEM_WINDOWS) || defined(__MINGW32__)
#	include <windows.h>
#	define SLEEP_MS(ms) \
		Sleep(ms)
#else
#	include <thread>
#	include <chrono>
#	define SLEEP_MS(ms) \
		std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelRenderer.hpp"
#include "SidePanel.hpp"
#include "MovingEntity.hpp"
#include "Controls.hpp"
#include "HomeScreen.hpp"
#include "utils.hpp"

// Fallback in case the game wasn't compiled properly with cmake
#ifndef VERSION
#	define VERSION "unknown"
#endif
#ifndef COMMIT
#	define COMMIT "unknown"
#endif

using Game::TILE_SIZE;
using Game::MAIN_WINDOW_SHIFT;
using Game::LEVEL_WIDTH;
using Game::LEVEL_HEIGHT;

void play_game(sf::RenderWindow& window, const std::string& level_set, unsigned short start_level = 1);
void displayGetReady(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short lvnum);
Game::Level* advanceLevel(sf::RenderWindow& window, Game::LevelRenderer& lr, Game::SidePanel& panel);

static sf::Font interlevel_font;

int main(int argc, char **argv) {
	// Argument parsing
	std::string levelSet = "";
	bool args_ended = false;
	unsigned short start_level = 1;
	int i = 1;
	while (i < argc) {
		if (!args_ended && argv[i][0] == '-') {
			switch (argv[i][1]) {
			case '-':
				args_ended = true;
				break;
			case 'l':
				start_level = std::atoi(argv[++i]);
				break;
			case 'v':
				std::cout << "lifish v." << VERSION << " rev." << COMMIT << std::endl;	
				return 0;
			default:
				std::cout << "Usage: " << argv[0] << " [-l <levelnum>] [-v] [levelset.json]\n"
					  << "\t-l: start at level <levelnum>\n"
					  << "\t-v: print version and exit" << std::endl;
				return 1;
			}
		} else {
			levelSet = std::string(argv[i]);
		}
		++i;
	}

	// Initialize game global variables
	if (Game::init())
		std::clog << "Game successfully initialized. pwd = " << Game::pwd << std::endl;
	else {
		std::cerr << "Game failed to initialize!" << std::endl;
		return 1;
	}

	if (levelSet.length() < 1)
		levelSet = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");

	// Create the rendering window
	sf::RenderWindow window(sf::VideoMode(
				Game::WINDOW_WIDTH, 
				Game::WINDOW_HEIGHT), "Lifish v." VERSION );

	// Home screen
	Game::HomeScreen home;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::MouseMoved:
				home.triggerMouseOver(sf::Mouse::getPosition(window));
				break;
			case sf::Event::MouseButtonReleased:
				{
					const auto clicked = home.triggerMouseClick(sf::Mouse::getPosition(window));
					if (clicked == "start") {
						play_game(window, levelSet, start_level);
						break;
					} else if (clicked == "exit") {
						return 0;
					}
				}
			default:
				break;
			}
		}
		window.clear();
		home.draw(window);
		window.display();
	}

	return 0;
}

void play_game(sf::RenderWindow& window, const std::string& level_set, unsigned short start_level) {
	bool vsync = false;

	// Parse the level set
	Game::LevelSet levelset(level_set);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	// Create the level renderer and side panel and attach the 1st level to them
	Game::Level *level = levelset.getLevel(start_level);
	levelset.printInfo();
	Game::LevelRenderer lr {
		new Game::Player(sf::Vector2f(0, 0), 1),
		new Game::Player(sf::Vector2f(0, 0), 2)
	};
	lr.setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	lr.loadLevel(level);
	Game::SidePanel panel(&lr);

	// Load the interlevel font
	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
	interlevel_font.loadFromFile(fontname);

#ifdef RELEASE
	displayGetReady(window, panel, 1);
#endif
	auto music = level->getMusic();
	music->play();

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

	int cycle = 0;
	lr.resetClocks();

	// Main game cycle
	while (window.isOpen()) {
		// If all players are dead, scroll down the GAME OVER text
		if (gameOverTriggered) {
			if (lr.isGameOverEnded()) {
				return;
			}
		} else if (levelClearTriggered) {
			const auto time = levelClearClock.getElapsedTime().asSeconds();
			if (time >= 4) {
				music->stop();	
				level = advanceLevel(window, lr, panel);
				music = level->getMusic();
				music->play();
				lr.resetClocks();
				players = lr.getPlayers();
				levelClearTriggered = false;
				for (auto& player : players)
					if (player != nullptr)
						player->setWinning(false);
			} else if (time >= 1) {	
				for (auto& player : players)
					if (player != nullptr)
						player->setWinning(true);
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
					for (auto& player : players)
						if (player != nullptr) {
							player->kill();
							player->setRemainingLives(0);
						}
					break;
#ifndef RELEASE
				case sf::Keyboard::Key::Add:
					{
						short n = level->getLevelNum() + 1;
						if (n > levelset.getLevelsNum())
							n = 1;
						levelClearTriggered = false;
						for (auto& player : players)
							if (player != nullptr)
								player->setWinning(false);
						lr.loadLevel(level = levelset.getLevel(n));
						players = lr.getPlayers();
						break;
					}
				case sf::Keyboard::Key::Subtract: 
					{
						short n = level->getLevelNum() - 1;
						if (n < 1)
							n = levelset.getLevelsNum();
						levelClearTriggered = false;
						for (auto& player : players)
							if (player != nullptr)
								player->setWinning(false);
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

		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
	}
}

void displayGetReady(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short lvnum) {
	std::stringstream ss;
	ss << "LEVEL " << lvnum;
	sf::Text text(ss.str(), interlevel_font, 13);
	text.setPosition(Game::center(text.getGlobalBounds()));

	window.clear();
	window.draw(text);

	text.setString("GET READY!");
	const auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

	window.draw(text);
	panel.draw(window);
	window.display();

	SLEEP_MS(3000);
}

Game::Level* advanceLevel(sf::RenderWindow& window, Game::LevelRenderer& lr, Game::SidePanel& panel) {
	// Display the time bonus on screen
	auto time_bonus = lr.getTimeLeft();
	
	sf::Text time_bonus_text("TIME BONUS!", interlevel_font, 13);
	if (time_bonus > 0) {
		auto bounds = time_bonus_text.getGlobalBounds();
		time_bonus_text.setPosition(Game::center(bounds));

		window.clear();
		window.draw(time_bonus_text);
		
		sf::Text points_text("0", interlevel_font, 13);
		bounds = points_text.getGlobalBounds();
		points_text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

		window.draw(points_text);
		panel.draw(window);
		window.display();

		SLEEP_MS(2000);

		// Assign time bonus points
		unsigned int points = 0;
		auto givePoints = [&lr] (int amount) {
			auto players = lr.getPlayers();
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
				if (players[i] != nullptr)
					Game::score[i] += amount;
		};
		auto level = lr.getLevel();
		while (time_bonus > 0) {
			if (time_bonus > 60) {
				time_bonus -= 60;
				level->setTime(time_bonus);
				points += 1000;
				givePoints(1000);
			} else {
				--time_bonus;
				level->setTime(time_bonus);
				points += 100;
				givePoints(100);
			}
			points_text.setString(Game::to_string(points));
			points_text.setPosition(Game::center(points_text.getGlobalBounds()) + sf::Vector2f(0.f, 2 * bounds.height));

			window.clear();
			window.draw(time_bonus_text);
			window.draw(points_text);
			lr.resetClocks();
			panel.draw(window);
			window.display();
			SLEEP_MS(50);
		}

		SLEEP_MS(2000);
	}

	const auto level = lr.getLevel();
	const auto levelSet = level->getLevelSet();
	short lvnum = level->getLevelNum();

	if (lvnum == levelSet->getLevelsNum()) {
		// TODO: WIN!
		return nullptr;
	} else {
		++lvnum;
	}

	displayGetReady(window, panel, lvnum);

	lr.loadLevel(levelSet->getLevel(lvnum));

	return const_cast<Game::Level*>(lr.getLevel());
}
