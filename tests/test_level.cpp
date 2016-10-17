#include <memory>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <SFML/Window.hpp>
#include "LevelManager.hpp"
#include "WinLoseHandler.hpp"
#include "Interactable.hpp"
#include "Player.hpp"
#include "Music.hpp"
#include "SidePanel.hpp"
#include "LevelSet.hpp"
#include "UI.hpp"
#include "game.hpp"
#include "Options.hpp"
#include "Controllable.hpp"
#include "ControlsScreen.hpp"
#include "PreferencesScreen.hpp"
#include "DebugRenderer.hpp"
#include "MusicManager.hpp"
#include "GameCache.hpp"

#ifdef MULTITHREADED
#	ifdef SFML_SYSTEM_LINUX
#		include <X11/Xlib.h>
#	endif
#	include <thread>
#endif

using namespace Game;

#ifdef MULTITHREADED
static void rendering_loop(sf::RenderWindow& window, const Game::LevelManager& lm, 
		const Game::SidePanel& sidePanel, const Game::UI::UI& ui)
{
	while (window.isOpen() && !Game::terminated) {
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
	std::string levelSetName = "";
	unsigned short start_level = 1;
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
				levelSetName = std::string(argv[i]);
			}
			++i;
		}
	}
	
	// Create the MusicManager (in a local scope)
	Game::MusicManager mm;
	Game::musicManager = &mm;

	if (!Game::init()) {
		std::cerr << "[ FATAL ] Failed to initialize the game!" << std::endl;
		return 1;
	}

	if (levelSetName.length() < 1)
		levelSetName = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	
	sf::RenderWindow window(sf::VideoMode(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), "test level");
	bool vsync = true;
	bool debug = false;
	window.setVerticalSyncEnabled(vsync);
	window.setJoystickThreshold(Game::JOYSTICK_INPUT_THRESHOLD);
	Game::options.showFPS = true;

	// Setup UI
	Game::UI::UI& ui = Game::UI::UI::getInstance();
	// load static screens
	ui.load(window, { "home.json", "about.json", "pause.json" });
	// load dynamic screens
	ui.add(new Game::UI::ControlsScreen(window));
	ui.add(new Game::UI::PreferencesScreen(window));
	// TODO
	ui.getScreenHandler().setCurrent("pause");

	// Load level set
	int lvnum = start_level;
	Game::LevelSet ls(levelSetName);
	if (lvnum > ls.getLevelsNum())
		lvnum %= ls.getLevelsNum();
	std::unique_ptr<Game::Level> level(ls.getLevel(lvnum));

	Game::LevelManager lm;
	// Create the players
	auto players = lm.createNewPlayers();
	for (auto p : players)
		p->get<Game::Controllable>()->setWindow(&window);

	SidePanel sidePanel(lm);
	WinLoseHandler wlHandler(lm);

	lm.setLevel(*level);

	// Adjust the origin to make room for side panel
	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);
	lm.setOrigin(origin);

	// Setup the music
	Game::options.musicVolume = 0; // FIXME
	Game::options.soundsVolume = 0; // FIXME
	Game::musicManager->set(level->get<Game::Music>()->getMusic())
		.setVolume(Game::options.musicVolume)
		.play();

	int cycle = 0;
	bool was_ui_active = false;

	lm.resume();
	
#ifdef MULTITHREADED
	window.setActive(false);
	const sf::Time frame_time_limit = sf::seconds(1 / 60.);
	sf::Clock frame_clock;
	std::thread rendering_thread(rendering_loop, std::ref(window),
			std::cref(lm), std::cref(sidePanel), std::cref(ui));
#endif

	while (window.isOpen() && !Game::terminated) {
		sf::Event event;
		
		///// EVENT LOOP /////
		
		if (ui.isActive()) 
			ui.handleEvents(window);
		else {
			while (window.pollEvent(event)) {
				switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					switch (event.key.code) {
					case sf::Keyboard::P:
						if (ui.toggleActive()) {
							lm.pause();
							was_ui_active = true;
							Game::musicManager->pause();
						} else {
							Game::musicManager->play();
						}
						break;
					case sf::Keyboard::Escape:
						for (auto player : players) {
							player->setRemainingLives(0);
							player->get<Killable>()->kill();
						}
						break;
#ifndef MULTITHREADED
					case sf::Keyboard::V:
						window.setVerticalSyncEnabled(vsync = !vsync);
						break;
#endif
#ifndef RELEASE
					case sf::Keyboard::Q:
						Game::terminated = true;
						break;
					case sf::Keyboard::M:
						lm.getEntities().apply([] (Game::Entity *e) {
							auto en = dynamic_cast<Game::Enemy*>(e);
							if (en) en->setMorphed(!en->isMorphed());
						});
						break;
					case sf::Keyboard::N:
						lm.getEntities().apply([] (Game::Entity *e) {
							auto en = dynamic_cast<Game::Enemy*>(e);
							//auto en = dynamic_cast<Game::BreakableWall*>(e);
							if (en) en->get<Game::Killable>()->kill();
						});
						break;
					case sf::Keyboard::Add:
						lvnum = level->getInfo().levelnum + 1;
						if (lvnum > ls.getLevelsNum())
							lvnum = 1;
						level.reset(ls.getLevel(lvnum));
						Game::musicManager->set(level->get<Game::Music>()->getMusic())
							.setVolume(Game::options.musicVolume).play();
						lm.setLevel(*level.get());
						break;
					case sf::Keyboard::Subtract:
						lvnum = level->getInfo().levelnum - 1;
						if (lvnum < 1) 
							lvnum = ls.getLevelsNum();
						level.reset(ls.getLevel(lvnum));
						Game::musicManager->set(level->get<Game::Music>()->getMusic())
							.setVolume(Game::options.musicVolume).play();
						lm.setLevel(*level.get());
						break;
					case sf::Keyboard::L:
						if (lm.isPaused())
							lm.update();
						else
							lm.pause();
						break;
					case sf::Keyboard::K:
						if (!lm.isPaused())
							lm.pause();
						else
							lm.resume();
						break;
					case sf::Keyboard::G:
						debug = !debug;
						break;
#endif
					default: 
						break;
					}
				default: 
					break;
				}
			}
		} // end event loop

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

			// TODO: handle win/loss
			wlHandler.handleWinLose();

			// Update level
			if (!lm.isPaused())
				lm.update();

			sidePanel.update();
			// Draw everything
			window.clear();
			window.draw(lm);
			window.draw(sidePanel);
			if (debug)
				Debug::DebugRenderer::drawColliders(window, lm.getEntities());
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
			if (!lm.isPaused())
				lm.update();
		}
		sf::sleep(frame_time_limit - frame_clock.restart());
#endif

		// Garbage-collect sounds
		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
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
