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
#include "DebugRenderer.hpp"
#include "MusicManager.hpp"
#include "GameCache.hpp"
#include "SHCollisionDetector.hpp"

#ifdef MULTITHREADED
#	ifdef SFML_SYSTEM_LINUX
#		include <X11/Xlib.h>
#	endif
#	include <thread>
#endif

using namespace Game;

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
				std::cout << "lifish v." VERSION " rev." COMMIT;
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
				return 0;
			default:
				std::cout << "Usage: " << argv[0] << " [-l <levelnum>] [-v] [levelset.json]\n"
					  << "\t-l: start at level <levelnum>\n"
					  << "\t-v: print version and exit" << std::endl;
				return 1;
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

static void toggle_pause_game(UI::UI& ui, LevelManager& lm, bool& was_ui_active) {
	if (ui.toggleActive()) {
		lm.pause();
		was_ui_active = true;
		Game::musicManager->pause();
	} else {
		Game::musicManager->play();
	}
}

static void print_cd_stats(LevelManager& lm) {
	const auto& dbgStats = lm.getCollisionDetector().getStats();
#ifndef RELEASE
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(4)
		<< "#checked: " << std::setw(5) << dbgStats.counter.safeGet("checked")
		<< " | tot: " << std::setw(8) << dbgStats.timer.safeGet("tot")
		<< " | tot_narrow: " << std::setw(8) << dbgStats.timer.safeGet("tot_narrow")
		<< " | setup: " << std::setw(8) << dbgStats.timer.safeGet("setup") 
		<< " | average: " << std::setw(8) 
			<< dbgStats.timer.safeGet("tot_narrow")/dbgStats.counter.safeGet("checked")
		<< std::resetiosflags(std::ios::showbase) << std::endl;
#endif
}

#ifdef MULTITHREADED
static void rendering_loop(sf::RenderWindow& window, const Game::LevelManager& lm, 
		const Game::SidePanel& sidePanel, const Game::UI::UI& ui)
{
	while (window.isOpen() && !Game::terminated) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Resized:
					window.setView(keep_ratio(event.size, sf::Vector2u(
							Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT)));
					break;
				default:
					break;
			}
		}
		window.clear();
		if (ui.isActive()) {
			window.draw(ui);	
		} else {
			window.draw(lm);
			window.draw(sidePanel);
		}
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

	if (levelset_name.length() < 1)
		levelset_name = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	
	// Create the game window
	Game::windowSize = sf::Vector2u(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	sf::RenderWindow window(
			sf::VideoMode(Game::windowSize.x, Game::windowSize.y),
			"Lifish " VERSION " (test)");
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
	ui.setSize(Game::windowSize);

	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json" });
	// load dynamic screens
	ui.add(new Game::UI::ControlsScreen(window, Game::windowSize));
	ui.add(new Game::UI::PreferencesScreen(window, Game::windowSize));
	// TODO
	//ui.getScreenHandler().setCurrent("pause");

	// Initialize game
	GameContext game(window, levelset_name, start_level);

	// Adjust the origin to make room for side panel
	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);
	game.setOrigin(origin);

	// Setup the music
	//Game::options.musicVolume = 0; // FIXME
	//Game::options.soundsVolume = 0; // FIXME
	Game::musicManager->set(level->get<Game::Music>()->getMusic())
		.setVolume(Game::options.musicVolume)
		.play();

	bool was_ui_active = false;

	lm.resume();

#ifdef MULTITHREADED
	// Start the rendering thread
	window.setActive(false);
	const sf::Time frame_time_limit = sf::seconds(1 / 60.);
	sf::Clock frame_clock;
	std::thread rendering_thread(rendering_loop, std::ref(window),
			std::cref(lm), std::cref(sidePanel), std::cref(ui));
#endif

	unsigned short cycle = 0;
	while (window.isOpen() && !Game::terminated) {
		sf::Event event;
		
		///// EVENT LOOP /////
		
		if (ui.isActive()) 
			ui.handleEvents(window);
		else
			game.handleEvents(window);

		///// LOGIC & RENDERING /////

#ifndef MULTITHREADED
		if (ui.isActive()) {
			ui.update();
			window.draw(ui);
		} else {
			if (was_ui_active) {
				lm.resume();
				was_ui_active = false;
			}

			// Handle win / loss cases
			wlHandler.handleWinLose();
			if (wlHandler.getState() == WinLoseHandler::State::ADVANCING_LEVEL) {
				// Give bonus points/handle continues/etc
				wlHandler.advanceLevel(window, sidePanel);
				if (wlHandler.getState() == WinLoseHandler::State::GAME_WON) {
					// TODO
				}
				
				for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
					players[i] = lm.getPlayer(i + 1);
				level = ls.getLevel(level->getInfo().levelnum + 1);
				lm.setLevel(*level);
				Game::musicManager->set(level->get<Game::Music>()->getMusic())
					.setVolume(Game::options.musicVolume)
					.play();
				continue;
			}

			// Update level
			if (!lm.isPaused())
				lm.update();

#ifndef RELEASE
			if (cycle++ % 50 == 0 && (debug >> DBG_PRINT_CD_STATS) == 1)
				print_cd_stats(lm);
#endif

			sidePanel.update();
			// Draw everything
			window.clear();
			window.draw(lm);
			window.draw(sidePanel);
			if ((debug >> DBG_DRAW_COLLIDERS) & 1)
				Debug::DebugRenderer::drawColliders(window, lm.getEntities());
			if ((debug >> DBG_DRAW_SH_CELLS) & 1)
				Debug::DebugRenderer::drawSHCells(window,
						static_cast<const Game::SHCollisionDetector&>(
							lm.getCollisionDetector()));
		}
		Game::maybeShowFPS(window);
		window.display();
#else
		if (ui.isActive()) {
			ui.update();
		} else {
			if (was_ui_active) {
				lm.resume();
				was_ui_active = false;
			}

			// TODO: handle win/loss
			wlHandler.handleWinLose(level);
			
			if (!lm.isPaused())
				lm.update();
		}
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
