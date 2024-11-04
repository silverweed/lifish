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
#include "FPSDisplayer.hpp"
#include "GameCache.hpp"
#include "GameContext.hpp"
#include "Interactable.hpp"
#include "JoystickManager.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "LoadScreen.hpp"
#include "HighScoreScreen.hpp"
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
#include "language.hpp"
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
	int fps = -1;
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
		try {
			lif::LevelSet ls(args.levelsetName);
			std::cout << "--------------\r\nLevelset info:\r\n--------------\r\n"
				<< ls.toString() << std::endl;
			std::exit(0);
		} catch (const std::exception&) {
			std::cerr << "Error: file \"" << args.levelsetName
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

static void setupUI(lif::ui::UI& ui, sf::RenderWindow& window, const std::string& levelsetName) {
	ui.setSize(lif::options.windowSize);

	// Setup dynamic texts for static screens
	ui.setDynamicText("FULL_VERSION", lif::gameInfo());
	ui.setDynamicText("LEVEL_SET", levelsetName);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json", "win.json", "error.json" });
	// create dynamic screens
	ui.add<lif::ui::ControlsScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::PreferencesScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::LoadScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::SaveScreen>(window, lif::options.windowSize);
	ui.add<lif::ui::HighScoreScreen>(window, lif::options.windowSize);
}

#if defined(_MSC_VER) && defined(RELEASE)
// No console in release mode on windows
#	pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
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

	if (!lif::loadL10nStrings(lif::getAsset("l10n", lif::LOCALIZATION_STRINGS_FILE))) {
		std::cerr << "[ ERROR ] Failed to load localization strings!\n";
	}

	// Must be done AFTER lif::init() but BEFORE the UI initializes!
	lif::loadPreferences(lif::preferencesPath);

	if (args.muteSounds)
		lif::options.soundsVolume = 0;
	if (args.muteMusic)
		lif::options.musicVolume = 0;
	if (args.levelsetName.length() < 1)
		args.levelsetName = lif::assetDir + std::string("levels.json");

	// Create the game window
	lif::options.windowSize = sf::Vector2u(lif::WINDOW_WIDTH, lif::WINDOW_HEIGHT);
	lif::options.vsync = true;
	if (args.fps >= 0)
		lif::options.framerateLimit = args.fps;

	sf::RenderWindow window;
	createRenderWindow(window);

	lif::FPSDisplayer fpsDisplayer(sf::Vector2f(), lif::getAsset("fonts", lif::fonts::DEBUG_INFO));
#ifndef RELEASE
	lif::options.showFPS = true;
#endif

	// Setup window icon except on macOS so the Big Sur version from the app bundle is used instead
#if !defined(SFML_SYSTEM_MACOS)
	loadIcon(window);
#endif

	// Setup UI
	auto& ui = lif::ui::UI::getInstance();
	setupUI(ui, window, args.levelsetName);

	// Create pointer to game context
	std::unique_ptr<lif::GameContext> game;

	// Adjust the origin to make room for side panel
	const sf::Vector2f origin(-lif::SIDE_PANEL_WIDTH, 0);

	std::array<lif::WindowContext*, 4> contexts;
	contexts[lif::CTX_GAME] = game.get();
	contexts[lif::CTX_UI] = &ui;
	contexts[lif::CTX_INTERLEVEL] = nullptr;
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

		if (cur_context == &ui) {
			if (ui.mustQuitGame())
				game.reset();
			else if (lif::switchedLanguage) {
				lif::switchedLanguage = false;
				ui.rebuildScreens();
			}
		}

		// Check context switch
		cur_context = checkContextSwitch(window, contexts, cur_context, game, args);

		if (ui.mustSaveGame()) {
			const auto saveName = ui.getSaveName() + ".lifish";
			if (lif::SaveManager::saveGame(saveName, game->getLM()))
				std::cerr << "[ INFO ] saved game in " << saveName << "." << std::endl;
			else
				std::cerr << "[ WARNING ] could not save game in " << saveName << "." << std::endl;
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

		if (game && lif::options.showGameTimer)
			window.draw(game->getGameTimer());

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

		if (wasVSync != lif::options.vsync)
			window.setVerticalSyncEnabled(lif::options.vsync);

		window.setFramerateLimit(lif::options.framerateLimit);

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
			if (game->getLM().isGameOver()) {
				ui.setCurrent("home");
				game.reset();
			} else if (game->isGameWon()) {
				ui.setCurrent("win");
				game->restart();
			} else {
				ui.setCurrent("pause");
			}
			break;
		case lif::CTX_INTERLEVEL:
			if (cur_context == &ui) {
				// We got here from the start menu (via StartGame or LoadGame)
				int startLv = args.startLevel;
				if (!game) {
					// If we're going to load a game, make GameContext skip LevelManager initialization
					// on construction (it will initialize it on loadGame).
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
			if (cur_context == contexts[lif::CTX_INTERLEVEL])
			{
				// New level just started
				game->onLevelStart();
			}
			// Keep inputs disabled for a brief period after switching back in game.
			// This prevents accidental placement of bombs and similar.
			game->getLM().disableInputFor(sf::seconds(0.5));
			break;
		default:
			break;
		}
		cur_context = contexts[nc];
		cur_context->setActive(true);
	}
	return cur_context;
}
