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
#include "LevelRenderer.hpp"
#include "SidePanel.hpp"
#include "MovingEntity.hpp"
#include "Controls.hpp"
#include "HomeScreen.hpp"
#include "PreferencesScreen.hpp"
#include "ControlsScreen.hpp"
#include "AboutScreen.hpp"
#include "PauseScreen.hpp"
#include "SaveManager.hpp"
#include "utils.hpp"
#ifdef SFML_SYSTEM_WINDOWS
#	include "win/dialogs.hpp"
#else
#	include "nfd.h"
#endif

using Game::TILE_SIZE;
using Game::MAIN_WINDOW_SHIFT;
using Game::LEVEL_WIDTH;
using Game::LEVEL_HEIGHT;

/** Bitmask used for handleScreenEvents() */
enum {
	HOME_SCREEN        = 1,
	PREFERENCES_SCREEN = 1 << 1,
	CONTROLS_SCREEN    = 1 << 2,
	ABOUT_SCREEN       = 1 << 3,
	PAUSE_SCREEN       = 1 << 4,
};

enum class GameAction {
	START_GAME,
	LOAD_GAME,
	SAVE_GAME,
	EXIT,
	DO_NOTHING
};

/** Starts the game on `window`, using `level_set` and starting at level `start_level` */
static void play_game(sf::RenderWindow& window, const std::string& level_set,
		Game::LevelRenderer& lr, unsigned short start_level = 1);

/** Displays a file browser to locate a lifish save file. Returns either the file name or ""
 *  if the loading was canceled / had errors.
 */
static std::string display_load_dialog();
static std::string display_save_dialog();

/** Displays the menu, starting with `rootScreen` and exiting when it should change to a
 *  screen which is not enabled. All screens are enabled by default.
 */
static GameAction handleScreenEvents(sf::RenderWindow& window, int rootScreen, int enabledScreens = ~0);
static void displayGetReady(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short lvnum);
static bool displayContinue(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short playernum);
static Game::Level* advanceLevel(sf::RenderWindow& window, Game::LevelRenderer& lr, Game::SidePanel& panel);

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

	while (window.isOpen()) {
		switch (handleScreenEvents(window, HOME_SCREEN, ~0 & ~PAUSE_SCREEN)) {
		case GameAction::START_GAME:
			{
				Game::LevelRenderer lr {
					new Game::Player(sf::Vector2f(0, 0), 1),
					new Game::Player(sf::Vector2f(0, 0), 2)
				};
				play_game(window, levelSet, lr, start_level);
				break;
			}
		case GameAction::LOAD_GAME:
			{
				const auto fname = display_load_dialog();
				if (fname.length() > 0) {
					Game::LevelRenderer lr {
						new Game::Player(sf::Vector2f(0, 0), 1),
						new Game::Player(sf::Vector2f(0, 0), 2)
					};

					if (Game::SaveManager::loadGame(fname, lr, start_level))
						play_game(window, levelSet, lr, start_level);
					else
						std::cerr << "Couldn't load game from " << fname 
							  << ": the save file is probably corrupt." << std::endl;
				}
				break;
			}
		case GameAction::EXIT:
			window.close();
			break;
		default:
			break;
		}
	}

	return 0;
}

void play_game(sf::RenderWindow& window, const std::string& level_set,
		Game::LevelRenderer& lr, unsigned short start_level)
{
	bool vsync = false;

	for (unsigned short i = 0; i < Game::playerContinues.size(); ++i)
		Game::playerContinues[i] = Game::INITIAL_CONTINUES;

	// Parse the level set
	Game::LevelSet levelset(level_set);
	std::clog << "Loaded " << levelset.getLevelsNum() << " levels." << std::endl;

	// Create the level renderer and side panel and attach the 1st level to them
	Game::Level *level = levelset.getLevel(start_level);
	levelset.printInfo();

	lr.setOrigin(sf::Vector2f(-MAIN_WINDOW_SHIFT, 0.f));
	lr.loadLevel(level);
	Game::SidePanel panel(&lr);

	// Load the interlevel font
	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
	interlevel_font.loadFromFile(fontname);

#ifdef RELEASE
	displayGetReady(window, panel, 1);
#endif
	Game::music = level->getMusic();
	Game::playMusic();

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
	bool levelClearTriggered = false, 
	     levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;
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
				level = advanceLevel(window, lr, panel);
				Game::music = level->getMusic();
				Game::playMusic();
				lr.resetClocks();
				players = lr.getPlayers();
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
						levelClearSoundPlayed = false;
						playerWinSoundPlayed = false;
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
				case sf::Keyboard::P:
					{
						// Pause
						if (Game::music != nullptr)
							Game::music->pause();
						lr.pauseClocks();
						auto action = GameAction::DO_NOTHING;
						do {
							action = handleScreenEvents(window, PAUSE_SCREEN,
									PAUSE_SCREEN | PREFERENCES_SCREEN | CONTROLS_SCREEN);
							if (action == GameAction::SAVE_GAME) {
								const auto fname = display_save_dialog();
								if (fname.length() > 0) {
									Game::SaveManager::saveGame(fname, lr);
								}
								// TODO: display some confirm screen
							};
						} while (action == GameAction::SAVE_GAME);
						lr.resumeClocks();
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
					for (auto& use : Game::useJoystick)
						if (use == short(id))
							use = -1;
					break;
				}
			default:
				break;
			}
		}

		Game::Direction dir[] = { Game::Direction::NONE, Game::Direction::NONE };
		for (unsigned int i = 0; i < 2; ++i) {
			if (players[i] == nullptr) continue;

			if (window.hasFocus()) {
				if (Game::useJoystick[i] >= 0) {
					const short joystick = Game::useJoystick[i];
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

		bool maybe_all_dead = false;
		for (unsigned short i = 0; i < 2; ++i) {
			if (players[i] == nullptr) {
				continue;
			}
			if (players[i]->isAligned()) {
				players[i]->prevAlign = Game::tile(players[i]->getPosition());
				if (window.hasFocus() && !players[i]->isDying())
					if ((Game::useJoystick[i] >= 0 
							&& sf::Joystick::isButtonPressed(
								Game::useJoystick[i], 
								Game::joystickBombKey[i]))
							|| sf::Keyboard::isKeyPressed(
								Game::playerControls[i][Game::Control::BOMB]))
						lr.dropBomb(i);
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
						maybe_all_dead = !lr.removePlayer(i + 1);
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
					if (displayContinue(window, panel, i+1)) {
						all_dead = false;
						players[i] = new Game::Player(sf::Vector2f(0, 0), i+1);
						lr.setPlayer(i+1, players[i]);
						--Game::playerContinues[i];
					} else {
						Game::playerContinues[i] = 0;
					}
				}
			}
			if (!all_dead) {
				// Restart level
				displayGetReady(window, panel, level->getLevelNum());
				lr.resetClocks();
				lr.loadLevel(level);
				levelClearTriggered = false;
				levelClearSoundPlayed = false;
				playerWinSoundPlayed = false;
				Game::playMusic();
				continue;
			} else if (!gameOverTriggered) {
				lr.triggerGameOver();
				gameOverTriggered = true;
			}
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
			window.draw(fps_text);
			window.draw(vsync_text);
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

bool displayContinue(sf::RenderWindow& window, Game::SidePanel& panel, const unsigned short playernum) {
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
	
	sf::Text yesText("YES", interlevel_font, 15);
	yesText.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 4 * bounds.height));
	yesText.setColor(sf::Color::Red);

	bounds = yesText.getGlobalBounds();
	text.setString(" / ");
	text.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));
	texts.push_back(text);

	bounds = text.getGlobalBounds();
	sf::Text noText("NO", interlevel_font, 15);
	noText.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));

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

	bool yesSelected = true;

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
					yesSelected = true;
					break;
				case sf::Keyboard::Right:
					yesSelected = false;
					break;
				case sf::Keyboard::Return:
					return yesSelected;
				default:
					break;
				}
			default:
				break;
			}
		}

		if (yesSelected) {
			yesText.setColor(sf::Color::Red);
			noText.setColor(sf::Color::White);
		} else {
			yesText.setColor(sf::Color::White);
			noText.setColor(sf::Color::Red);
		}

		window.clear();
		panel.draw(window);
		drawTexts();
		window.draw(yesText);
		window.draw(noText);
		window.display();	
	}

	return false;
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
			if (displayContinue(window, panel, i+1)) {
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

	displayGetReady(window, panel, lvnum);

	lr.loadLevel(levelSet->getLevel(lvnum));

	return const_cast<Game::Level*>(lr.getLevel());
}

GameAction handleScreenEvents(sf::RenderWindow& window, int rootScreen, int enabledScreens) {
	// All possible screens
	static struct {
		Game::HomeScreen        &home = Game::HomeScreen::getInstance();
		Game::PreferencesScreen &preferences = Game::PreferencesScreen::getInstance();
		Game::ControlsScreen    &controls = Game::ControlsScreen::getInstance();
		Game::AboutScreen       &about = Game::AboutScreen::getInstance();
		Game::PauseScreen       &pause = Game::PauseScreen::getInstance();
	} screens;

	// The currently displayed screen
	Game::Screen *cur_screen = nullptr;
	
	switch (rootScreen) {
	case HOME_SCREEN:
		cur_screen = &screens.home;
		break;
	case PREFERENCES_SCREEN: 
		cur_screen = &screens.preferences;
		break;
	case CONTROLS_SCREEN: 
		cur_screen = &screens.controls;
		break;
	case ABOUT_SCREEN:
		cur_screen = &screens.about;
		break;
	case PAUSE_SCREEN:
		cur_screen = &screens.pause;
		break;
	default:
		throw std::invalid_argument("Called handleScreenEvents with non-existing screen!");
	}

	// The enabled screens
	std::unordered_set<Game::Screen*> enabled_screens;
	if (enabledScreens & HOME_SCREEN) 
		enabled_screens.insert(&screens.home);
	if (enabledScreens & PREFERENCES_SCREEN)
		enabled_screens.insert(&screens.preferences);
	if (enabledScreens & CONTROLS_SCREEN) 
		enabled_screens.insert(&screens.controls);
	if (enabledScreens & ABOUT_SCREEN)
		enabled_screens.insert(&screens.about);
	if (enabledScreens & PAUSE_SCREEN)
		enabled_screens.insert(&screens.pause);

	window.clear();
	cur_screen->draw(window);

	auto find_parent = [&enabled_screens] (Game::Screen *screen) -> Game::Screen* {
		for (const auto& parent : screen->getParents()) {
			if (enabled_screens.find(parent) != enabled_screens.end())
				return parent;
		}
		return nullptr;
	};

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseMoved:
				cur_screen->triggerMouseOver(
						window.mapPixelToCoords(sf::Mouse::getPosition(window)));
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Escape) {
					const auto parent = find_parent(cur_screen);
					if (parent != nullptr)
						cur_screen = parent;
					else
						return GameAction::DO_NOTHING;
				}
				break;
			case sf::Event::MouseButtonReleased:
				{
					const auto clicked = cur_screen->triggerMouseClick(
							window.mapPixelToCoords(sf::Mouse::getPosition(window)));
					if (clicked == "start") {
						return GameAction::START_GAME;
					} else if (clicked == "load") {
						return GameAction::LOAD_GAME;
					} else if (clicked == "save") {
						return GameAction::SAVE_GAME;
					} else if (clicked == "preferences") {
						if (enabledScreens & PREFERENCES_SCREEN)
							cur_screen = &screens.preferences;
						break;
					} else if (clicked == "preferences::music_volume_down") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::LOWER);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::music_volume_up") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::RAISE);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::music_mute_toggle") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::MUTE_TOGGLE);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::sounds_volume_down") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::LOWER);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::sounds_volume_up") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::RAISE);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::sounds_mute_toggle") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::MUTE_TOGGLE);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::controls") {
						if (enabledScreens & CONTROLS_SCREEN)
							cur_screen = &screens.controls;
						break;
					} else if (clicked == "controls::p1") {
						screens.controls.selectPlayer(1);
						break;
					} else if (clicked == "controls::p2") {
						screens.controls.selectPlayer(2);
						break;
					} else if (Game::startsWith(clicked, "controls::change_")) {
						screens.controls.changeControl(window, clicked);
						break;
					} else if (clicked == "controls::joystick_toggle") {
						screens.controls.toggleJoystick();
						break;
					} else if (clicked == "about") {
						if (enabledScreens & ABOUT_SCREEN)
							cur_screen = &screens.about;
						break;
					} else if (clicked == "exit") {
						const auto parent = find_parent(cur_screen);
						if (parent != nullptr)
							cur_screen = parent;
						else
							return GameAction::EXIT;
					}
					break;
				}
			default:
				break;
			}
		}
		window.clear();
		cur_screen->draw(window);
		window.display();
	}
	return GameAction::DO_NOTHING;
}


std::string display_load_dialog() {
#if defined(SFML_SYSTEM_WINDOWS)
	return Game::Dialog::openFile();
#elif !defined(HAVE_NFD)
	std::cerr << "[ WARNING ] lifish was compiled without GTK support:\n"
		     "if you want to load a game, place a file named `save.lifish`\n"
		     "in the directory where the lifish executable resides."
		  << std::endl;
	return "save.lifish";
#else
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("lifish", Game::pwd, &outPath);
	switch (result) {
	case NFD_OKAY:
		{
			std::string path(outPath);
			free(outPath);
			return path;
		}
	case NFD_ERROR:
		std::cerr << "Error opening file: " << NFD_GetError() << std::endl;
		// fallthrough
	default:
		return "";
	}
#endif
}

std::string display_save_dialog() {
#if defined(SFML_SYSTEM_WINDOWS)
	return Game::Dialog::saveFile();
#elif !defined(HAVE_NFD)
	std::cerr << "[ WARNING ] lifish was compiled without GTK support:\n"
		     "the game will be saved in `" << Game::pwd << Game::DIRSEP << "save.lifish`."
		  << std::endl;
	return "save.lifish";
#else
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog("lifish", Game::pwd, &outPath);
	switch (result) {
	case NFD_OKAY:
		{
			std::string path(outPath);
			free(outPath);
			return path;
		}
	case NFD_ERROR:
		std::cerr << "Error opening file: " << NFD_GetError() << std::endl;
		// fallthrough
	default:
		return "";
	}
#endif
}
