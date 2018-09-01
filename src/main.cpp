/*!
 * Lifish
 * @copyright 2017, Giacomo Parolini
 * @author [Giacomo Parolini](https://github.com/silverweed)
 * @see https://github.com/silverweed/lifish to download the source code
 *
 * This game is licensed under the Lifish License, available at
 * https://silverweed.github.io/lifish-license.txt
 * or in the LICENSE file in this repository's root directory.
*/
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <SFML/Window.hpp>
#include "LevelManager.hpp"
#include "WinLoseHandler.hpp"
#include "Interactable.hpp"
#include "Player.hpp"
#include "JoystickManager.hpp"
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
#include "CutscenePlayer.hpp"
#include "CutsceneBuilder.hpp"
#include "FPSDisplayer.hpp"

#ifdef MULTITHREADED
#	ifdef SFML_SYSTEM_LINUX
#		include <X11/Xlib.h>
#	endif
#	include <thread>
#endif

#ifndef RELEASE
#	include "Stats.hpp"
#	include "DebugPainter.hpp"
#	include <cassert>
#endif

struct MainArgs {
	short start_level;
	std::string levelset_name;
	bool mute_sounds;
	bool mute_music;
	int fps;
#ifndef RELEASE
	bool start_from_home;
#endif
};

inline lif::WindowContext* checkContextSwitch(sf::RenderWindow& window,
		std::array<lif::WindowContext*, 4>& contexts, lif::WindowContext *cur_context,
		std::unique_ptr<lif::GameContext>& game, const MainArgs& args);

static void parseArgs(int argc, char **argv, /* out */ MainArgs& args) {
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
			case 'f':
				if (i < argc - 1)
					args.fps = std::atoi(argv[++i]);
				else
					std::cerr << "[ WARNING ] Expected numeral after -f flag" << std::endl;
				break;
#ifndef RELEASE
			case 'u':
				args.start_from_home = true;
				break;
#endif
			case 'v':
				std::cout << lif::gameInfo();
				std::exit(0);
			case 'i':
				print_level_info = true;
				break;
			default:
				std::cout << "Usage: " << argv[0]
				          << " [-l <levelnum>] [-v] [-f <fps>] [levelset.json]\r\n"
				          << "\t-l: start at level <levelnum>\r\n"
				          << "\t-i: print info about <levelset.json> and exit\r\n"
				          << "\t-s: start with sounds muted\r\n"
				          << "\t-m: start with music muted\r\n"
				          << "\t-f: set framerate limit to <fps>\r\n"
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

static void loadIcon(sf::Window& window) {
	sf::Image iconImg;
	if (iconImg.loadFromFile(lif::getAsset("graphics", "icon.png"))) {
		auto pixels = iconImg.getPixelsPtr();
		auto size = iconImg.getSize();
		window.setIcon(size.x, size.y, pixels);
	}
}

static void setupUI(lif::ui::UI& ui, sf::RenderWindow& window) {
	ui.setSize(lif::options.windowSize);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json" });
	// create dynamic screens
	ui.add<lif::ui::ControlsScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::PreferencesScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::LoadScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::SaveScreen>(window, lif::options.windowSize);
}

#ifdef MULTITHREADED
static void renderingLoop(sf::RenderWindow& window) {
	static const sf::Vector2f fps_pos(
			lif::WINDOW_WIDTH - lif::TILE_SIZE * 8;
			lif::WINDOW_HEIGHT - lif::TILE_SIZE);
	while (!lif::terminated) {
		window.clear();
		window.draw(*lif::curContext);
		lif::maybeShowFPS(window, fps_pos);
		window.display();
	}
	window.close();
}
#endif

int main(int argc, char **argv) {
	// Duplicate stderr to file for logging
	//std::ofstream errFile(".lifish_err.txt");
	//auto errbuf = std::cerr.rdbuf();
	//std::cerr.rdbuf(errFile.rdbuf());

#if defined(MULTITHREADED) && defined(SFML_SYSTEM_LINUX)
	XInitThreads();
#endif
	// Argument parsing
	MainArgs args;
	args.start_level = 1;
	args.fps = 120;
	args.levelset_name = "";
	args.mute_music = true; // FIXME
	args.mute_sounds = true; // FIXME
#ifndef RELEASE
	args.start_from_home = false; // FIXME
#endif
	parseArgs(argc, argv, args);

	// Create the MusicManager
	lif::MusicManager mm;
	lif::musicManager = &mm;

#ifndef RELEASE
	// Create the DebugPainter
	lif::DebugPainter debugPainter;
	lif::debugPainter = &debugPainter;
#endif

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
	lif::options.framerateLimit = args.fps;
	window.setFramerateLimit(lif::options.framerateLimit);
	window.setJoystickThreshold(lif::JOYSTICK_INPUT_THRESHOLD);
	const sf::Vector2f fps_pos(
			lif::WINDOW_WIDTH - lif::TILE_SIZE * 8,
			lif::WINDOW_HEIGHT - lif::TILE_SIZE);
	lif::FPSDisplayer fpsDisplayer(fps_pos, lif::getAsset("fonts", lif::fonts::DEBUG_INFO));
#ifndef RELEASE
	lif::options.showFPS = true;
#endif

	// Setup icon
	loadIcon(window);

	// Setup UI
	auto& ui = lif::ui::UI::getInstance();
	setupUI(ui, window);

	// Create pointer to game context
	std::unique_ptr<lif::GameContext> game;

	// Adjust the origin to make room for side panel
	const sf::Vector2f origin(-lif::SIDE_PANEL_WIDTH, 0);

	// Create cutscene player
	lif::CutscenePlayer cutscenePlayer;

	std::array<lif::WindowContext*, 4> contexts;
	contexts[lif::CTX_GAME] = game.get();
	contexts[lif::CTX_UI] = &ui;
	contexts[lif::CTX_INTERLEVEL] = nullptr;
	contexts[lif::CTX_CUTSCENE] = &cutscenePlayer;
	// Note: this is always assumed non-null throughout the program
	lif::WindowContext *cur_context = contexts[lif::CTX_UI];
#ifndef RELEASE
	if (!args.start_from_home) {
		game.reset(new lif::GameContext(window, args.levelset_name, args.start_level));
		game->setOrigin(origin);
		// Adjust context pointers
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
#endif

	while (!lif::terminated) {

		///// EVENT LOOP /////

		lif::joystick::JoystickManager::getInstance().update();
		cur_context->handleEvents(window);

		if (cur_context == &ui && ui.mustQuitGame())
			game.reset();

		// Check context switch
		cur_context = checkContextSwitch(window, contexts, cur_context, game, args);

		if (ui.mustSaveGame()) {
			const auto saveName = ui.getSaveName() + ".lifish";
			lif::SaveManager::saveGame(saveName, game->getLM());
			std::cerr << "Saved game in " << saveName << "." << std::endl;
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

		fpsDisplayer.update();
		if (lif::options.showFPS)
			window.draw(fpsDisplayer);

		window.display();

#	ifndef RELEASE
		dbgStats.timer.end("draw");
		++cycle;
		if (lif::options.printDrawStats && cycle % 50 == 0) {
			std::ios::fmtflags flags(std::cout.flags());
			std::cout //<< std::setfill(' ') << std::scientific << std::setprecision(4)
				<< ">> Draw: " << std::setw(6) << dbgStats.timer.safeGet("draw") * 1000
				<< " ms" << std::endl;
			std::cout.flags(flags);
		}
#	endif
#else
		// Just wait for the vsync
		sf::sleep(frame_time_limit - frame_clock.restart());
#endif
	} // end game loop

#ifndef MULTITHREADED
	window.close();
#else
	rendering_thread.join();
#endif
	// Perform cleanup
	mm.stop();
	lif::cache.finalize();

	// Restore stream buffers
	//std::cerr.rdbuf(errbuf);

	return lif::exitCode;
}

lif::WindowContext* checkContextSwitch(sf::RenderWindow& window,
		std::array<lif::WindowContext*, 4>& contexts, lif::WindowContext *cur_context,
		std::unique_ptr<lif::GameContext>& game, const MainArgs& args)
{
	assert(cur_context != nullptr);
	const int nc = cur_context->getNewContext();
	if (nc >= 0) {
		auto& ui = *static_cast<lif::ui::UI*>(contexts[lif::CTX_UI]);
		cur_context->setActive(false);
		cur_context->resetNewContext();
		switch (nc) {
		case lif::CTX_UI:
			if (cur_context == game.get() && game->getLM().isGameOver()) {
				ui.setCurrent("home");
				game.reset();
			} else
				ui.setCurrent("pause");
			break;
		case lif::CTX_INTERLEVEL:
			if (cur_context == &ui) {
				// We got here from the start menu (via StartGame or LoadGame)
				int startLv = args.start_level;
				if (!game) {
					// Game just started: create a new GameContext
					game.reset(new lif::GameContext(window,
						args.levelset_name, args.start_level));
					// Adjust the origin to make room for side panel
					const sf::Vector2f origin(-lif::SIDE_PANEL_WIDTH, 0);
					game->setOrigin(origin);
				}
				if (ui.mustLoadGame()) {
					const auto save_data = ui.getLoadedData();
					game->loadGame(save_data);
					startLv = save_data.level;
				}
				contexts[lif::CTX_GAME] = game.get();
				contexts[lif::CTX_INTERLEVEL] = &game->getWLHandler()
					.getInterlevelContext();
				game->getLM().pause();
				static_cast<lif::InterlevelContext*>(contexts[lif::CTX_INTERLEVEL])
					->setGettingReady(startLv);
			}
			break;
		case lif::CTX_GAME:
			if (cur_context == contexts[lif::CTX_INTERLEVEL]
					&& game->getLM().getLevel()->getInfo().levelnum == args.start_level)
			{
				// New level just started
				game->onLevelStart();
			}
			// Keep inputs disabled for a brief period after switching back in game.
			// This prevents accidental placement of bombs and similar.
			game->getLM().disableInputFor(sf::seconds(0.5));
			break;
		case lif::CTX_CUTSCENE:
			{
				auto& cutscenePlayer = *static_cast<lif::CutscenePlayer*>(contexts[lif::CTX_CUTSCENE]);
				cutscenePlayer.reset();
				std::string cutsceneToPlay = "";
				using WLState = lif::WinLoseHandler::State;
				if (game->getWLHandler().getState() == WLState::ADVANCED_LEVEL) {
					cutsceneToPlay = game->getLM().getLevel()->getInfo().cutscenePre;
					cutscenePlayer.setNewContext(lif::CTX_GAME);
				} else {
					cutsceneToPlay = game->getLM().getLevel()->getInfo().cutscenePost;
					cutscenePlayer.setNewContext(lif::CTX_INTERLEVEL);
					static_cast<lif::InterlevelContext*>(contexts[lif::CTX_INTERLEVEL])
						->setAdvancingLevel();
				}
				cutscenePlayer.addCutscenes(lif::CutsceneBuilder::fromJson(cutsceneToPlay));
				cutscenePlayer.play();
				break;
			}
		default:
			break;
		}
		cur_context = contexts[nc];
		cur_context->setActive(true);
#ifdef MULTITHREADED
		lif::curContext = cur_context;
#endif
	}
	return cur_context;
}
