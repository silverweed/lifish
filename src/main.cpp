/*!
 * Lifish
 * @copyright 2022, silverweed
 * @author [silverweed](https://github.com/silverweed)
 * @see https://github.com/silverweed/lifish to download the source code
 *
 * This game is licensed under the Lifish License, available at
 * https://silverweed.github.io/lifish-license.txt
 * or in the LICENSE file in this repository's root directory.
*/
#include "Bonusable.hpp"
#include "Boss.hpp"
#include "Controllable.hpp"
#include "ControlsScreen.hpp"
#include "CutsceneBuilder.hpp"
#include "CutscenePlayer.hpp"
#include "FPSDisplayer.hpp"
#include "GameCache.hpp"
#include "GameContext.hpp"
#include "Interactable.hpp"
#include "JoystickManager.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "LoadScreen.hpp"
#include "Music.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "PreferencesScreen.hpp"
#include "SaveScreen.hpp"
#include "SidePanel.hpp"
#include "Time.hpp"
#include "UI.hpp"
#include "WinLoseHandler.hpp"
#include "contexts.hpp"
#include "game.hpp"
#include "preferences_persistence.hpp"
#include "utils.hpp"
#include <SFML/Window.hpp>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#ifndef RELEASE
#	include "Stats.hpp"
#	include "DebugPainter.hpp"
#	include "FadeoutTextManager.hpp"
#	include <cassert>
#endif

struct MainArgs {
	short startLevel = 1;
	std::string levelsetName;
	bool muteSounds = false;
	bool muteMusic = false;
	int fps = 60;
#ifndef RELEASE
	bool startFromHome = false;
#endif
};

static lif::WindowContext* checkContextSwitch(sf::RenderWindow& window,
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
					args.startLevel = std::atoi(argv[++i]);
				else
					std::cerr << "[ WARNING ] Expected numeral after -l flag" << std::endl;
				break;
			case 's':
				args.muteSounds = !args.muteSounds;
				break;
			case 'm':
				args.muteMusic = !args.muteMusic;
				break;
			case 'f':
				if (i < argc - 1)
					args.fps = std::atoi(argv[++i]);
				else
					std::cerr << "[ WARNING ] Expected numeral after -f flag" << std::endl;
				break;
#ifndef RELEASE
			case 'u':
				args.startFromHome = true;
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
			args.levelsetName = std::string(argv[i]);
		}
		++i;
	}

	if (print_level_info) {
		lif::LevelSet ls;
		if (ls.loadFromFile(args.levelsetName)) {
			std::cout << "--------------\r\nLevelset info:\r\n--------------\r\n"
				<< ls.toString() << std::endl;
			std::exit(0);
		} else {
			std::cerr << "Error: levels file \"" << args.levelsetName
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

static void createRenderWindow(sf::Window& window) {

	const sf::VideoMode videoMode = lif::options.fullscreen
		? lif::options.videoMode
		: sf::VideoMode(lif::options.windowSize.x, lif::options.windowSize.y);

	window.create(
		videoMode,
		"Lifish " VERSION,
		sf::Style::Default | (lif::options.fullscreen * sf::Style::Fullscreen));

	window.setFramerateLimit(lif::options.framerateLimit);
	window.setVerticalSyncEnabled(lif::options.vsync);
	window.setJoystickThreshold(lif::JOYSTICK_INPUT_THRESHOLD);

	static_cast<sf::RenderWindow&>(window).setView(lif::keepRatio(
		sf::Vector2f(videoMode.width, videoMode.height), lif::options.windowSize));
}

static void setupUI(lif::ui::UI& ui, sf::RenderWindow& window) {
	ui.setSize(lif::options.windowSize);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json", "error.json" });
	// create dynamic screens
	ui.add<lif::ui::ControlsScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::PreferencesScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::LoadScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::SaveScreen>(window, lif::options.windowSize);

	// Setup dynamic texts for static screens
	{
		const auto fullVersion =
			"lifish v." VERSION " rev." COMMIT
#ifdef RELEASE
				" RELEASE"
#endif
#ifndef ARCH
				" (unknown arch)"
#else
				" (" ARCH " bit)"
#endif
				;

		ui.setDynamicText("FULL_VERSION", fullVersion);
	}
}

int main(int argc, char **argv) {

	// Argument parsing
	MainArgs args;
	parseArgs(argc, argv, args);

	// Create the MusicManager
	lif::MusicManager mm;
	lif::musicManager = &mm;

	// Initialize game variables
	if (!lif::init()) {
		std::cerr << "[ FATAL ] Failed to initialize the game!" << std::endl;
		return 1;
	}

#ifndef RELEASE
	// Create debug utils
	lif::debug::DebugPainter debugPainter;
	lif::debugPainter = &debugPainter;

	lif::debug::FadeoutTextManager fadeoutTextMgr(
			*lif::cache.loadFont(lif::getAsset("fonts", lif::fonts::DEBUG_INFO)), sf::seconds(5));
	lif::fadeoutTextMgr = &fadeoutTextMgr;
#endif

	// Must be done AFTER lif::init() but BEFORE the UI initializes!
	lif::loadPreferences(lif::PREFERENCES_SAVE_FILE_NAME);

	if (args.muteSounds)
		lif::options.soundsVolume = 0;
	if (args.muteMusic)
		lif::options.musicVolume = 0;
	if (args.levelsetName.length() < 1)
		args.levelsetName = std::string(lif::pwd) + lif::DIRSEP + std::string("levels.json");

	// Create the game window
	lif::options.windowSize = sf::Vector2u(lif::WINDOW_WIDTH, lif::WINDOW_HEIGHT);
	lif::options.vsync = true;
	lif::options.framerateLimit = args.fps;

	sf::RenderWindow window;
	createRenderWindow(window);

	lif::FPSDisplayer fpsDisplayer(sf::Vector2f(), lif::getAsset("fonts", lif::fonts::DEBUG_INFO));
#ifndef RELEASE
	lif::options.showFPS = true;
#endif

	// Setup icon
	loadIcon(window);

	// Setup UI
	auto& ui = lif::ui::UI::getInstance();
	setupUI(ui, window);
	ui.setDynamicText("LEVEL_SET", args.levelsetName);

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
	if (!args.startFromHome) {
		game.reset(new lif::GameContext(window, args.levelsetName, args.startLevel));
		game->setOrigin(origin);
		// Adjust context pointers
		contexts[lif::CTX_GAME] = game.get();
		contexts[lif::CTX_INTERLEVEL] = &game->getWLHandler()
						.getInterlevelContext();
		cur_context = contexts[lif::CTX_GAME];

		if (!game || !game->isLevelSetGood()) {
			cur_context = contexts[lif::CTX_UI];
			ui.setCurrent("error");
		}
	}
	lif::debug::Stats dbgStats;
	unsigned cycle = 0;
#endif

	bool wasWindowFullscreen = lif::options.fullscreen;
	bool wasVSync = lif::options.vsync;
	sf::VideoMode curVideoMode = lif::options.videoMode;

	while (!lif::terminated) {

		lif::time.update();

		///// EVENT LOOP /////

		lif::joystick::JoystickManager::getInstance().update();
		cur_context->handleEvents(window);

		if (cur_context == &ui && ui.mustQuitGame())
			game.reset();

		// Check context switch
		cur_context = checkContextSwitch(window, contexts, cur_context, game, args);

		if (ui.mustSaveGame()) {
			const auto saveName = ui.getSaveName() + ".lifish";
			lif::saveGame(saveName, game->getLM());
			std::cerr << "Saved game in " << saveName << "." << std::endl;
		}

		///// LOGIC LOOP /////

		cur_context->update();
#ifndef RELEASE
		fadeoutTextMgr.update();
#endif

		///// RENDERING LOOP //////

#ifndef RELEASE
		dbgStats.timer.start("draw");
#endif
		window.clear();
		window.draw(*cur_context);
#ifndef RELEASE
		window.draw(fadeoutTextMgr);

#endif
		fpsDisplayer.update(lif::options.windowSize);
		if (lif::options.showFPS)
			window.draw(fpsDisplayer);

		window.display();

#ifndef RELEASE
		dbgStats.timer.end("draw");
		++cycle;
		if (lif::options.printDrawStats && cycle % 50 == 0) {
			std::ios::fmtflags flags(std::cout.flags());
			std::cout //<< std::setfill(' ') << std::scientific << std::setprecision(4)
				<< ">> Draw: " << std::setw(6) << dbgStats.timer.safeGet("draw") * 1000
				<< " ms" << std::endl;
			std::cout.flags(flags);
		}
#endif

		// Handle fullscreen
		if (wasWindowFullscreen != lif::options.fullscreen || curVideoMode != lif::options.videoMode) {
			createRenderWindow(window);
		}

		// Handle vsync change
		if (wasVSync != lif::options.vsync) {
			window.setFramerateLimit(lif::options.vsync ? lif::options.framerateLimit : 0);
			window.setVerticalSyncEnabled(lif::options.vsync);
		}

		wasWindowFullscreen = lif::options.fullscreen;
		wasVSync = lif::options.vsync;
		curVideoMode = lif::options.videoMode;

	} // end game loop

	window.close();

	// Perform cleanup
	mm.stop();
	lif::cache.finalize();

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
				int startLv = args.startLevel;
				if (!game) {
					// If we're going to load a game, make GameContext skip LevelManager
					// initialization on construction (it will initialize it on loadGame).
					const auto& lsName = ui.mustLoadGame() ? "" : args.levelsetName;

					// Game just started: create a new GameContext
					game.reset(new lif::GameContext(window, lsName, args.startLevel));
					if (!game || (!ui.mustLoadGame() && !game->isLevelSetGood())) {
						cur_context = contexts[lif::CTX_UI];
						ui.setCurrent("error");
						return cur_context;
					}

					// Adjust the origin to make room for side panel
					const sf::Vector2f origin(-lif::SIDE_PANEL_WIDTH, 0);
					game->setOrigin(origin);
				}
				if (ui.mustLoadGame()) {
					const auto save_data = ui.getLoadedData();
					ui.setDynamicText("LEVEL_SET", save_data.levelSet);
					game->loadGame(save_data);
					if (!game->isLevelSetGood()) {
						cur_context = contexts[lif::CTX_UI];
						ui.setCurrent("error");
						return cur_context;
					}
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
					&& game->getLM().getLevel()->getInfo().levelnum == args.startLevel)
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
	}
	return cur_context;
}
