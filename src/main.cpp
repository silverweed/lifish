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
#include <unordered_set>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include "Level.hpp"
#include "LevelSet.hpp"
#include "LevelManager.hpp"
#include "SidePanel.hpp"
#include "MovingEntity.hpp"
#include "Controls.hpp"
#include "ScreenHandler.hpp"
#include "SaveManager.hpp"
#include "Options.hpp"
#include "Dialogs.hpp"
#include "utils.hpp"

using Game::TILE_SIZE;
using Game::MAIN_WINDOW_SHIFT;
using Game::LEVEL_WIDTH;
using Game::LEVEL_HEIGHT;
using Game::HOME_SCREEN;
using Game::PREFERENCES_SCREEN;
using Game::CONTROLS_SCREEN;
using Game::ABOUT_SCREEN;
using Game::PAUSE_SCREEN;

/** Starts the game on `window`, using `level_set` and starting at level `start_level` */
static void play_game(sf::RenderWindow& window, const std::string& level_set,
		Game::LevelManager& lr, unsigned short start_level = 1);

static void display_get_ready(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short lvnum);
static bool display_continue(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short playernum);
static Game::Level* advance_level(sf::RenderWindow& window, Game::LevelManager& lr, Game::SidePanel& panel);
static Game::Direction[] get_directions(sf::RenderWindow& window, 
		const std::array<Game::Player*, Game::MAX_PLAYERS>& players);

static sf::Font interlevel_font;

int main(int argc, char **argv) {
	// Argument parsing
	std::string levelSetName = "";
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
				std::cout << "lifish v." VERSION " rev." COMMIT;
#ifndef ARCH
				std::cout << " (unknown arch)" << std::endl;
#else
				std::cout << " (" ARCH " bit)" << std::endl;
#endif
#ifdef HAVE_NFD
				std::cout << "    | NFD support: yes" << std::endl;
#else
				std::cout << "    | NFD support: no" << std::endl;
#endif
				return 0;
			default:
				std::cout << "Usage: " << argv[0] << " [-l <levelnum>] [-v] [levelset.json]\n"
					  << "\t-l: start at level <levelnum>\n"
					  << "\t-v: print version and exit" << std::endl;
				return 1;
			}
		} else {
			levelSetName = std::string(argv[i]);
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

	if (levelSetName.length() < 1)
		levelSetName = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");

	// Create the rendering window
	sf::RenderWindow window(sf::VideoMode(
				Game::WINDOW_WIDTH, 
				Game::WINDOW_HEIGHT), "Lifish v." VERSION);

	window.setVerticalSyncEnabled(true);

	while (window.isOpen()) {
		switch (ScreenHandler::handleScreenEvents(window, HOME_SCREEN, ~0 & ~PAUSE_SCREEN)) {
		case Game::Action::START_GAME:
			{
				Game::LevelManager lr {
					new Game::Player(sf::Vector2f(0, 0), 1),
					new Game::Player(sf::Vector2f(0, 0), 2)
				};
				Game::playerContinues.fill(Game::INITIAL_CONTINUES);

				play_game(window, levelSetName, lr, start_level);

				break;
			}
		case Game::Action::LOAD_GAME:
			{
				const auto fname = Game::display_load_dialog();
				if (fname.length() > 0) {
					Game::LevelManager lr {
						new Game::Player(sf::Vector2f(0, 0), 1),
						new Game::Player(sf::Vector2f(0, 0), 2)
					};

					if (Game::SaveManager::loadGame(fname, lr, start_level))
						play_game(window, levelSetName, lr, start_level);
					else
						std::cerr << "Couldn't load game from " << fname 
							  << ": the save file is probably corrupt." << std::endl;
				}

				break;
			}
		case Game::Action::EXIT:
			window.close();
			break;
		default:
			break;
		}
	}

	return 0;
}

void play_game(sf::RenderWindow& window, const std::string& levelSetName,
		Game::LevelManager& lm, unsigned short start_level)
{
	// Parse the level set
	Game::LevelSet levelset(levelSetName);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	// Create the level manager and side panel and attach the 1st level to them
	Game::Level *level = levelset.getLevel(start_level);

	lm.get<Game::LevelRenderer>()->setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	lm.loadLevel(level);

	// Create the side panel
	Game::SidePanel panel(&lm);

	// Load the interlevel font (FIXME)
	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
	interlevel_font.loadFromFile(fontname);


#ifdef RELEASE
	display_get_ready(window, panel, 1);
#endif
	Game::music = level->get<Game::Music>()->getMusic();
	Game::playMusic();

	auto players = lm.getPlayers();
	bool gameOverTriggered = false;
	bool levelClearTriggered = false, 
	     levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;
	sf::Clock levelClearClock;

	int cycle = 0;
	lm.resetClocks();

	// Main game cycle
	while (window.isOpen()) {
		// If all players are dead, scroll down the GAME OVER text
		if (gameOverTriggered) {
			if (lm.isGameOverEnded()) {
				return;
			}
		} else if (levelClearTriggered) {
			const auto time = levelClearClock.getElapsedTime().asSeconds();
			if (time >= 4) {
				level = advance_level(window, lm, panel);
				Game::music = level->get<Game::Music>()->getMusic();
				Game::playMusic();
				lm.resetClocks();
				players = lm.getPlayers();
				levelClearTriggered = false;
				levelClearSoundPlayed = false;
				playerWinSoundPlayed = false;
				for (auto& player : players)
					if (player != nullptr)
						player->setWinning(false);

			} else if (time >= 1.8 && !playerWinSoundPlayed) {
				for (auto& player : players)
					if (player != nullptr) {
						Game::cache.playSound(player->getSoundFile(Game::Sounds::WIN));
						player->setWinning(true);
					}

				playerWinSoundPlayed = true;

			} else if (time >= 1 && !levelClearSoundPlayed) {
				Game::stopMusic();
				Game::cache.playSound(Game::getAsset("test", Game::LEVEL_CLEAR_SOUND));
				levelClearSoundPlayed = true;
			}
		}

		// Event loop
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Key::Escape:
					for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
						auto player = players[i];
						if (player != nullptr) {
							player->kill();
							Game::cache.playSound(player->getSoundFile(Game::Sounds::DEATH));
							player->setRemainingLives(0);
						}
						Game::playerContinues[i] = 0;
					}
					break;
#ifndef RELEASE
				case sf::Keyboard::Key::Add:
					{
						short n = level->getLevelNum() + 1;
						if (n > levelset.getLevelsNum())
							n = 1;
						levelClearTriggered = false;
						levelClearSoundPlayed = false;
						playerWinSoundPlayed = false;
						for (auto& player : players)
							if (player != nullptr)
								player->setWinning(false);
						delete level;
						lm.loadLevel(level = levelset.getLevel(n));
						players = lm.getPlayers();
						break;
					}
				case sf::Keyboard::Key::Subtract: 
					{
						short n = level->getLevelNum() - 1;
						if (n < 1)
							n = levelset.getLevelsNum();
						levelClearTriggered = false;
						levelClearSoundPlayed = false;
						playerWinSoundPlayed = false;
						for (auto& player : players)
							if (player != nullptr)
								player->setWinning(false);
						delete level;
						lm.loadLevel(level = levelset.getLevel(n));
						players = lm.getPlayers();
						break;
					}
#endif
				case sf::Keyboard::Key::F:
					Game::options.showFPS = !Game::options.showFPS;
					break;
				case sf::Keyboard::P:
					{
						// Pause
						if (Game::music != nullptr)
							Game::music->pause();
						lm.pauseClocks();
						auto action = Game::Action::DO_NOTHING;
						do {
							action = ScreenHandler::handleScreenEvents(window, PAUSE_SCREEN,
									PAUSE_SCREEN | PREFERENCES_SCREEN | CONTROLS_SCREEN);
							if (action == Game::Action::SAVE_GAME) {
								const auto fname = Game::display_save_dialog();
								if (fname.length() > 0) {
									Game::SaveManager::saveGame(fname, lm);
								}
								// TODO: display some confirm screen
							};
						} while (action == Game::Action::SAVE_GAME);
						lm.resumeClocks();
						Game::playMusic();
						break;
					}
				default:
					break; 
				}
				break;
			case sf::Event::JoystickDisconnected:
				{
					const auto id = event.joystickConnect.joystickId;
					for (auto& use : Game::options.useJoystick)
						if (use == short(id))
							use = -1;
					break;
				}
			default:
				break;
			}
		}

		auto dir = get_directions(window, players);
		lm.checkHurryUp();

		if (lm.isExtraGame()) {
			lm.checkExtraGameEnd();
			lm.cycleLetters();
		}

		lm.checkLinesOfSight();
		lm.selectEnemyMoves();
		lm.makeBossesShoot();
		lm.checkBombExplosions();
		lm.checkExplosionHits();
		lm.detectCollisions();

		// Check players hurt / death; make players move
		bool maybe_all_dead = true;
		for (unsigned short i = 0; i < 2; ++i) {
			if (players[i] == nullptr) {
				continue;
			}
			maybe_all_dead = false;

			if (players[i]->isAligned()) {
				players[i]->prevAlign = Game::tile(players[i]->getPosition());
				if (window.hasFocus() && !players[i]->isDying())
					if ((Game::options.useJoystick[i] >= 0 
							&& sf::Joystick::isButtonPressed(
								Game::options.useJoystick[i], 
								Game::joystickBombKey[i]))
							|| sf::Keyboard::isKeyPressed(
								Game::playerControls[i][Game::Control::BOMB]))
						lm.dropBomb(i);
			}

			if (players[i]->isHurt()) {
				players[i]->prepareHurtAnimation();
				if (!players[i]->playHurtAnimation()) {
					players[i]->setHurt(false);
				}
			} else if (players[i]->isDying()) {
				players[i]->prepareDeathAnimation();
				if (!players[i]->playDeathAnimation()) {
					if (players[i]->getRemainingLives() <= 0) {
						maybe_all_dead = !lm.removePlayer(i + 1);
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
		if (maybe_all_dead) {
			bool all_dead = true;
			Game::stopMusic();
			// If all players are currently dead, resurrect any player
			// which has spare Continues.
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
				if (Game::playerContinues[i] > 0) {
					if (display_continue(window, panel, i+1)) {
						all_dead = false;
						players[i] = new Game::Player(sf::Vector2f(0, 0), i+1);
						lm.setPlayer(i+1, players[i]);
						--Game::playerContinues[i];
					} else {
						Game::playerContinues[i] = 0;
					}
				}
			}
			if (!all_dead) {
				// Restart level
				display_get_ready(window, panel, level->getLevelNum());
				lm.resetClocks();
				lm.loadLevel(level);
				levelClearTriggered = false;
				levelClearSoundPlayed = false;
				playerWinSoundPlayed = false;
				Game::playMusic();
				continue;
			} else if (!gameOverTriggered) {
				lm.triggerGameOver();
				gameOverTriggered = true;
			}
		} else if (!levelClearTriggered && lm.isLevelClear()) {
			levelClearTriggered = true;
			levelClearClock.restart();
		}

		lm.moveBullets();

		// Actually move enemies
		lm.applyEnemyMoves();

		window.clear();
		window.draw(lm);
		panel.draw(window);
		Game::maybeShowFPS(window);
		window.display();

		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
	}
}

void display_get_ready(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short lvnum) {
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

bool display_continue(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short playernum) {
	std::vector<sf::Text> texts;

	std::stringstream ss;
	ss << "PLAYER " << playernum << " CONTINUE?";
	sf::Text text(ss.str(), interlevel_font, 13);
	text.setPosition(Game::center(text.getGlobalBounds()));
	texts.push_back(text);

	ss.str("");
	ss << "(" << Game::playerContinues[playernum-1] << " remaining)";
	text.setString(ss.str());
	auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));
	texts.push_back(text);

	// Dummy text to get the correct bounds
	text.setString("YES / NO");
	text.setCharacterSize(15);
	bounds = text.getGlobalBounds();
	
	sf::Text yes_text("YES", interlevel_font, 15);
	yes_text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 4 * bounds.height));
	yes_text.setColor(sf::Color::Red);

	bounds = yes_text.getGlobalBounds();
	text.setString(" / ");
	text.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));
	texts.push_back(text);

	bounds = text.getGlobalBounds();
	sf::Text no_text("NO", interlevel_font, 15);
	no_text.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));

	text.setString("Arrows / Enter to select");
	text.setCharacterSize(10);
	text.setPosition(Game::center(text.getGlobalBounds()) + sf::Vector2f(0.f, 6 * bounds.height));
	texts.push_back(text);

	auto drawTexts = [&texts, &window] {
		for (const auto& text : texts)
			window.draw(text);
	};

	drawTexts();
	panel.draw(window);
	window.display();

	bool yes_selected = true;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				return false;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Left:
					yes_selected = true;
					break;
				case sf::Keyboard::Right:
					yes_selected = false;
					break;
				case sf::Keyboard::Return:
					return yes_selected;
				case sf::Keyboard::Key::F:
					Game::options.showFPS = !Game::options.showFPS;
					break;
				default:
					break;
				}
			default:
				break;
			}
		}

		if (yes_selected) {
			yes_text.setColor(sf::Color::Red);
			no_text.setColor(sf::Color::White);
		} else {
			yes_text.setColor(sf::Color::White);
			no_text.setColor(sf::Color::Red);
		}

		window.clear();
		panel.draw(window);
		drawTexts();
		window.draw(yes_text);
		window.draw(no_text);
		Game::maybeShowFPS(window);
		window.display();	
	}

	return false;
}

Game::Level* advance_level(sf::RenderWindow& window, Game::LevelManager& lr, Game::SidePanel& panel) {
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
		const auto time_bonus_sound = Game::getAsset("sounds", Game::TIME_BONUS_SOUND);
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
			Game::cache.playSound(time_bonus_sound);

			window.clear();
			window.draw(time_bonus_text);
			window.draw(points_text);
			lr.resetClocks();
			panel.draw(window);
			window.display();
			SLEEP_MS(60);
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

	// Resurrect any dead player which has a 'continue' left and
	// remove shield and speedy effects
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lr.getPlayer(i+1);
		if (player == nullptr && Game::playerContinues[i] > 0) {
			if (display_continue(window, panel, i+1)) {
				--Game::playerContinues[i];
				lr.setPlayer(i+1, new Game::Player(sf::Vector2f(0, 0), i+1));
			} else {
				Game::playerContinues[i] = 0;
			}
		} else if (player != nullptr) {
			player->giveShield(0);
			player->giveSpeedy(0);
		}
	}

	display_get_ready(window, panel, lvnum);

	delete level;
	lr.loadLevel(levelSet->getLevel(lvnum));

	return const_cast<Game::Level*>(lr.getLevel());
}

Game::Direction[] get_directions(sf::RenderWindow& window,
		const std::array<Game::Player*, Game::MAX_PLAYERS>& players)
{
	Game::Direction dir[] = { Game::Direction::NONE, Game::Direction::NONE };

	for (unsigned int i = 0; i < 2; ++i) {
		if (players[i] == nullptr) continue;

		if (window.hasFocus()) {
			if (Game::options.useJoystick[i] >= 0) {
				const short joystick = Game::options.useJoystick[i];
				const auto horizontal = sf::Joystick::getAxisPosition(joystick, sf::Joystick::X),
					   vertical = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Y);
				if (vertical < -Game::JOYSTICK_INPUT_THRESHOLD) 
					dir[i] = Game::Direction::UP;
				else if (vertical > Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::DOWN;
				else if (horizontal < -Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::LEFT;
				else if (horizontal > Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::RIGHT;
			} else {
				if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::UP]))
					dir[i] = Game::Direction::UP;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::LEFT]))
					dir[i] = Game::Direction::LEFT;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::DOWN]))
					dir[i] = Game::Direction::DOWN;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::RIGHT]))
					dir[i] = Game::Direction::RIGHT;
			}
		}

		if (players[i]->isAligned())
			players[i]->setDirection(dir[i]);
	}

	return dir;
}
