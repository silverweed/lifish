/*!
 * Lifish
 * @copyright The code is licensed under the [GNU General Public 
 * 	   License](https://github.com/silverweed/lifish/blob/master/LICENSE.txt):
 * 
 * 	This program is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 * 
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 * 
 * 	You should have received a copy of the GNU General Public License
 * 	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author [Giacomo Parolini](https://github.com/silverweed)
 * @see https://github.com/silverweed/lifish to download the source code
*/
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <SFML/Window.hpp>
#include "LevelManager.hpp"
#include "WinLoseHandler.hpp"
#include "Interactable.hpp"
#include "Player.hpp"
#include "Music.hpp"
#include "SidePanel.hpp"
#include "LevelSet.hpp"
#include "Bonusable.hpp"
#include "Boss.hpp"
#include "UI.hpp"
#include "game.hpp"
#include "Options.hpp"
#include "Controllable.hpp"
#include "ControlsScreen.hpp"
#include "PreferencesScreen.hpp"
#include "MusicManager.hpp"
#include "GameCache.hpp"
#include "GameContext.hpp"
#include "contexts.hpp"

#ifdef MULTITHREADED
#	ifdef SFML_SYSTEM_LINUX
#		include <X11/Xlib.h>
#	endif
#	include <thread>
#endif

using namespace Game;

static void print_version() {
	std::cout << "lifish v." VERSION " rev." COMMIT;
#ifdef RELEASE
	std::cout << " RELEASE";
#endif
#ifndef ARCH
	std::cout << " (unknown arch)" << std::endl;
#else
	std::cout << " (" ARCH " bit)" << std::endl;
#endif
#ifdef HAVE_NFD
	std::cout << "    | NFD support: yes" << std::endl;
#elif !defined(SFML_SYSTEM_WINDOWS)
	std::cout << "    | NFD support: no" << std::endl;
#endif
#ifdef MULTITHREADED 
	std::cout << "    | Multithreaded: yes" << std::endl;
#else
	std::cout << "    | Multithreaded: no" << std::endl;
#endif
}

static void parse_args(int argc, char **argv, 
	/* out */ unsigned short& start_level, /* out */ std::string& levelset_name)
{
	bool args_ended = false;
	int i = 1;
	while (i < argc) {
		if (!args_ended && argv[i][0] == '-') {
			switch (argv[i][1]) {
			case '-':
				args_ended = true;
				break;
			case 'l':
				if (i < argc - 1)
					start_level = std::atoi(argv[++i]);
				else
					std::cerr << "[ WARNING ] Expected numeral after -l flag" << std::endl;
				break;
			case 'v':
				print_version();
				exit(0);
			default:
				std::cout << "Usage: " << argv[0] << " [-l <levelnum>] [-v] [levelset.json]\n"
					  << "\t-l: start at level <levelnum>\n"
					  << "\t-v: print version and exit" << std::endl;
				exit(1);
			}
		} else {
			levelset_name = std::string(argv[i]);
		}
		++i;
	}
}

static void load_icon(sf::Window& window) {
	sf::Image iconImg;
	if (iconImg.loadFromFile(Game::getAsset("graphics", "icon.png"))) {
		auto pixels = iconImg.getPixelsPtr();
		auto size = iconImg.getSize();
		window.setIcon(size.x, size.y, pixels);
	}
}

#ifdef MULTITHREADED
static void rendering_loop(sf::RenderWindow& window) {
	while (window.isOpen() && !Game::terminated) {
		window.clear();
		window.draw(*Game::curContext);
		Game::maybeShowFPS(window);
		window.display();
	}
	if (window.isOpen())
		window.close();
}
#endif

int main(int argc, char **argv) {
#if defined(MULTITHREADED) && defined(SFML_SYSTEM_LINUX)
	XInitThreads();
#endif
	// Argument parsing
	unsigned short start_level = 1;
	std::string levelset_name = "";
	parse_args(argc, argv, start_level, levelset_name);
	
	// Create the MusicManager (in a local scope)
	Game::MusicManager mm;
	Game::musicManager = &mm;

	// Initialize game variables
	if (!Game::init()) {
		std::cerr << "[ FATAL ] Failed to initialize the game!" << std::endl;
		return 1;
	}

	Game::options.musicVolume = 0; // FIXME
	Game::options.soundsVolume = 0; // FIXME

	if (levelset_name.length() < 1)
		levelset_name = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	
	// Create the game window
	Game::options.windowSize = sf::Vector2u(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	sf::RenderWindow window(
			sf::VideoMode(Game::options.windowSize.x, Game::options.windowSize.y),
			"Lifish " VERSION);
	Game::options.vsync = true;
	Game::options.framerateLimit = 120;
	window.setFramerateLimit(Game::options.framerateLimit);
	window.setJoystickThreshold(Game::JOYSTICK_INPUT_THRESHOLD);
#ifndef RELEASE
	Game::options.showFPS = true;
#endif

	// Setup icon
	load_icon(window);

	// Setup UI
	Game::UI::UI& ui = Game::UI::UI::getInstance();
	ui.setSize(Game::options.windowSize);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json" });
	// load dynamic screens
	ui.add(new Game::UI::ControlsScreen(window, Game::options.windowSize));
	ui.add(new Game::UI::PreferencesScreen(window, Game::options.windowSize));

	// Create pointer to game context
	std::unique_ptr<GameContext> game;

	std::array<Game::WindowContext*, 3> contexts;
	contexts[Game::CTX_UI] = &ui;
	contexts[Game::CTX_GAME] = game.get();
	// Note: this is always assumed non-null throughout the program
	Game::WindowContext *cur_context = contexts[Game::CTX_UI];

	// Adjust the origin to make room for side panel
	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);

#ifdef MULTITHREADED
	Game::curContext = cur_context;
	// Start the rendering thread
	window.setActive(false);
	const sf::Time frame_time_limit = sf::seconds(1 / 60.);
	sf::Clock frame_clock;
	std::thread rendering_thread(rendering_loop, std::ref(window));
#endif

	while (window.isOpen() && !Game::terminated) {

		///// EVENT LOOP /////
		
		cur_context->handleEvents(window);

		// Check context switch
		{
			const int nc = cur_context->getNewContext();
			if (nc >= 0) {
				cur_context->setActive(false);
				cur_context->resetNewContext();
				switch (nc) {
				case Game::CTX_UI:
					if (cur_context == game.get() && game->getLM().isGameOver())
						ui.setCurrent("home");
					else
						ui.setCurrent("pause");
					break;
				case Game::CTX_INTERLEVEL:
					if (cur_context == &ui && ui.getCurrent() == "home") {
						// Game started: create a new GameContext
						game.reset(new Game::GameContext(window, levelset_name, start_level));
						game->setOrigin(origin);
						contexts[Game::CTX_GAME] = game.get();
						contexts[Game::CTX_INTERLEVEL] = &game->getWLHandler()
										.getInterlevelContext();
						game->getLM().pause();
						static_cast<Game::InterlevelContext*>(contexts[Game::CTX_INTERLEVEL])
											->setGettingReady(start_level);
					}
					break;
				}
				cur_context = contexts[nc];
				cur_context->setActive(true);
#ifdef MULTITHREADED
				Game::curContext = cur_context;
#endif
			}
		}

		///// LOGIC LOOP /////

		cur_context->update();

		///// RENDERING LOOP //////

#ifndef MULTITHREADED
		window.clear();
		window.draw(*cur_context);
		
		Game::maybeShowFPS(window);
		window.display();
#else
		// Just wait for the vsync
		sf::sleep(frame_time_limit - frame_clock.restart());
#endif
	} // end game loop

#ifndef MULTITHREADED
	if (window.isOpen())
		window.close();
#else
	rendering_thread.join();
#endif
	// Perform cleanup
	mm.stop();
	cache.finalize();

	return Game::exitCode;
}
