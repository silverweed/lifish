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
#include "LoadScreen.hpp"
#include "SaveScreen.hpp"
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

#ifndef RELEASE
#	include "Stats.hpp"
#endif

struct MainArgs {
	short start_level;
	std::string levelset_name;
	bool mute_sounds;
	bool mute_music;
#ifndef RELEASE
	bool start_from_home;
#endif	
};

static void parse_args(int argc, char **argv, /* out */ MainArgs& args) {
	bool args_ended = false;
	bool print_level_info = false;
	int i = 1;
	while (i < argc) {
		if (!args_ended && argv[i][0] == '-') {
			switch (argv[i][1]) {
			case '-':
				args_ended = true;
				break;
			case 'l':
				if (i < argc - 1)
					args.start_level = std::atoi(argv[++i]);
				else
					std::cerr << "[ WARNING ] Expected numeral after -l flag" << std::endl;
				break;
			case 's':
				args.mute_sounds = !args.mute_sounds;
				break;
			case 'm':
				args.mute_music = !args.mute_music;
				break;
#ifndef RELEASE
			case 'u':
				args.start_from_home = true;
				break;
#endif
			case 'v':
				std::cout << lif::game_info();
				std::exit(0);
			case 'i':
				print_level_info = true;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-l <levelnum>] [-v] [levelset.json]\r\n"
					  << "\t-l: start at level <levelnum>\r\n"
					  << "\t-i: print info about <levelset.json> and exit\r\n"
					  << "\t-s: start with sounds muted\r\n"
					  << "\t-m: start with music muted\r\n"
#ifndef RELEASE
					  << "\t-u: start in the home screen, not in game\r\n"
#endif
					  << "\t-v: print version and exit" << std::endl;
				std::exit(1);
			}
		} else {
			args.levelset_name = std::string(argv[i]);
		}
		++i;
	}

	if (print_level_info) {
		try {
			lif::LevelSet ls(args.levelset_name);
			std::cout << "--------------\r\nLevelset info:\r\n--------------\r\n"
				<< ls.toString() << std::endl;
			std::exit(0);
		} catch (std::exception ex) {
			std::cerr << "Error: file \"" << args.levelset_name
				<< "\" not found or with wrong format." << std::endl;
			std::exit(1);
		}
	}
}

static void load_icon(sf::Window& window) {
	sf::Image iconImg;
	if (iconImg.loadFromFile(lif::getAsset("graphics", "icon.png"))) {
		auto pixels = iconImg.getPixelsPtr();
		auto size = iconImg.getSize();
		window.setIcon(size.x, size.y, pixels);
	}
}

#ifdef MULTITHREADED
static void rendering_loop(sf::RenderWindow& window) {
	static const sf::Vector2f fps_pos(
			lif::SIDE_PANEL_WIDTH + (lif::LEVEL_WIDTH - 5.5) * lif::TILE_SIZE,
			lif::WINDOW_HEIGHT - lif::TILE_SIZE);
	while (window.isOpen() && !lif::terminated) {
		window.clear();
		window.draw(*lif::curContext);
		lif::maybeShowFPS(window, fps_pos);
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
	MainArgs args;
	args.start_level = 1;
	args.levelset_name = "";
	args.mute_music = true; // FIXME
	args.mute_sounds = true; // FIXME
#ifndef RELEASE
	args.start_from_home = false; // FIXME
#endif
	parse_args(argc, argv, args);
	
	// Create the MusicManager (in a local scope)
	lif::MusicManager mm;
	lif::musicManager = &mm;

	// Initialize game variables
	if (!lif::init()) {
		std::cerr << "[ FATAL ] Failed to initialize the game!" << std::endl;
		return 1;
	}

	if (args.mute_sounds)
		lif::options.soundsVolume = 0;
	if (args.mute_music)
		lif::options.musicVolume = 0;
	if (args.levelset_name.length() < 1)
		args.levelset_name = std::string(lif::pwd) + lif::DIRSEP + std::string("levels.json");
	
	// Create the game window
	lif::options.windowSize = sf::Vector2u(lif::WINDOW_WIDTH, lif::WINDOW_HEIGHT);
	sf::RenderWindow window(
			sf::VideoMode(lif::options.windowSize.x, lif::options.windowSize.y),
			"Lifish " VERSION);
	lif::options.vsync = true;
	lif::options.framerateLimit = 120;
	window.setFramerateLimit(lif::options.framerateLimit);
	window.setJoystickThreshold(lif::JOYSTICK_INPUT_THRESHOLD);
#ifndef RELEASE
	lif::options.showFPS = true;
#endif

	// Setup icon
	load_icon(window);

	// Setup UI
	lif::ui::UI& ui = lif::ui::UI::getInstance();
	ui.setSize(lif::options.windowSize);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json" });
	// load dynamic screens
	ui.add(new lif::ui::ControlsScreen(window, lif::options.windowSize));
	ui.add(new lif::ui::PreferencesScreen(window, lif::options.windowSize));
	ui.add(new lif::ui::LoadScreen(window, lif::options.windowSize));
	ui.add(new lif::ui::SaveScreen(window, lif::options.windowSize));

	// Create pointer to game context
	std::unique_ptr<lif::GameContext> game;

	// Adjust the origin to make room for side panel
	sf::Vector2f origin(-lif::SIDE_PANEL_WIDTH, 0);

	std::array<lif::WindowContext*, 3> contexts;
	contexts[lif::CTX_UI] = &ui;
	contexts[lif::CTX_GAME] = game.get();
	// Note: this is always assumed non-null throughout the program
	lif::WindowContext *cur_context = contexts[lif::CTX_UI];
#ifndef RELEASE
	if (!args.start_from_home) {
		game.reset(new lif::GameContext(window, args.levelset_name, args.start_level));
		game->setOrigin(origin);
		contexts[lif::CTX_GAME] = game.get();
		contexts[lif::CTX_INTERLEVEL] = &game->getWLHandler()
						.getInterlevelContext();
		cur_context = contexts[lif::CTX_GAME];
	}
	lif::debug::Stats dbgStats;
	unsigned cycle = 0;
#endif

#ifdef MULTITHREADED
	lif::curContext = cur_context;
	// Start the rendering thread
	window.setActive(false);
	const sf::Time frame_time_limit = sf::seconds(1 / 60.);
	sf::Clock frame_clock;
	std::thread rendering_thread(rendering_loop, std::ref(window));
#else
	const sf::Vector2f fps_pos(
			lif::SIDE_PANEL_WIDTH + (lif::LEVEL_WIDTH - 5.5) * lif::TILE_SIZE,
			lif::WINDOW_HEIGHT - lif::TILE_SIZE);
#endif

	while (window.isOpen() && !lif::terminated) {

		///// EVENT LOOP /////
		
		cur_context->handleEvents(window);

		// Check context switch
		const int nc = cur_context->getNewContext();
		if (nc >= 0) {
			cur_context->setActive(false);
			cur_context->resetNewContext();
			switch (nc) {
			case lif::CTX_UI:
				if (cur_context == game.get() && game->getLM().isGameOver())
					ui.setCurrent("home");
				else
					ui.setCurrent("pause");
				break;
			case lif::CTX_INTERLEVEL:
				if (cur_context == &ui && ui.getCurrent() == "home") {
					// Game started: create a new GameContext
					// TODO: support loading game
					game.reset(new lif::GameContext(window,
							args.levelset_name, args.start_level));
					game->setOrigin(origin);
					contexts[lif::CTX_GAME] = game.get();
					contexts[lif::CTX_INTERLEVEL] = &game->getWLHandler()
						.getInterlevelContext();
					game->getLM().pause();
					static_cast<lif::InterlevelContext*>(contexts[lif::CTX_INTERLEVEL])
						->setGettingReady(args.start_level);
				}
				break;
			}
			cur_context = contexts[nc];
			cur_context->setActive(true);
#ifdef MULTITHREADED
			lif::curContext = cur_context;
#endif
		}

		///// LOGIC LOOP /////

		cur_context->update();

		///// RENDERING LOOP //////

#ifndef MULTITHREADED
#	ifndef RELEASE
		dbgStats.timer.start("draw");
#	endif
		window.clear();
		window.draw(*cur_context);
		
		lif::maybeShowFPS(window, fps_pos);
		window.display();
#	ifndef RELEASE
		++cycle;
		if (lif::options.printDrawStats && cycle % 50 == 0) {
			std::ios::fmtflags flags(std::cerr.flags());
			std::cerr << std::setfill(' ') << std::scientific << std::setprecision(4)
				<< ">> Draw: " << std::setw(6) << dbgStats.timer.end("draw") << std::endl;
			std::cerr.flags(flags);
		}
#	endif
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
	lif::cache.finalize();

	return lif::exitCode;
}
