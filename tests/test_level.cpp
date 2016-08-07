#include <SFML/Window.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include "Level.hpp"
#include "LevelLoader.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "Bomb.hpp"
#include "Coin.hpp"
#include "Controllable.hpp"
#include "BossExplosion.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "TransparentWall.hpp"
#include "Enemy.hpp"
#include "EntityGroup.hpp"
#include "Teleport.hpp"
#include "Shooting.hpp"
#include "Player.hpp"
#include "Flash.hpp"
#include "Options.hpp"
#include "Explosion.hpp"
#include "Points.hpp"
#include "CollisionDetector.hpp"
#include "SidePanel.hpp"
#include "DroppingText.hpp"
#include "UI.hpp"
#include "ControlsScreen.hpp"
#include "PreferencesScreen.hpp"
#include "DebugRenderer.hpp"

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
	while (window.isOpen()) {
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
	ui.load(window, { "home.json", "about.json", "pause.json" });
	ui.add(new Game::UI::ControlsScreen(window));
	ui.add(new Game::UI::PreferencesScreen(window));
	ui.getScreenHandler().setCurrent("pause");

	// Load level set
	int lvnum = start_level;
	Game::LevelSet ls(levelSetName);
	std::unique_ptr<Game::Level> level(ls.getLevel(lvnum));

	Game::LevelManager lm;
	// Create the players
	auto players = lm.createNewPlayers();
	for (auto p : players)
		p->get<Game::Controllable>()->setWindow(&window);

	SidePanel sidePanel(lm);

	LevelLoader::load(*level.get(), lm);

	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);
	lm.setOrigin(origin);

	// Setup the music
	Game::options.musicVolume = 0; // FIXME
	Game::options.soundsVolume = 0; // FIXME
	Game::musicManager->set(level->get<Game::Music>()->getMusic()).setVolume(Game::options.musicVolume).play();

	int cycle = 0;
	bool was_ui_active = false;

	lm.get<Game::LevelTime>()->resume();
	
#ifdef MULTITHREADED
	window.setActive(false);
	std::thread rendering_thread(rendering_loop, std::ref(window), std::cref(lm), std::cref(sidePanel), std::cref(ui));
#endif

	while (window.isOpen() && !Game::terminated) {
		sf::Event event;
		
		// Event loop
		if (ui.isActive()) 
			ui.handleEvents(window);
		else while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Q:
					window.close();
					break;
				case sf::Keyboard::Y:
					window.setVerticalSyncEnabled(vsync = !vsync);
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
					level->setOrigin(origin);
					Game::musicManager->set(level->get<Game::Music>()->getMusic())
						.setVolume(Game::options.musicVolume).play();
					LevelLoader::load(*level.get(), lm);
					break;
				case sf::Keyboard::Subtract:
					lvnum = level->getInfo().levelnum - 1;
					if (lvnum < 1) 
						lvnum = ls.getLevelsNum();
					level.reset(ls.getLevel(lvnum));
					level->setOrigin(origin);
					Game::musicManager->set(level->get<Game::Music>()->getMusic())
						.setVolume(Game::options.musicVolume).play();
					LevelLoader::load(*level.get(), lm);
					break;
				case sf::Keyboard::L:
					if (lm.isPaused()) {
						std::cerr << "\n==== CYCLE " << cycle << " ====" << std::endl;
						lm.update();
					} else
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
				case sf::Keyboard::T:
					{
						auto dt = new Game::DroppingText(
								Game::getAsset("test", "gameover.png"),
								sf::Vector2i(311, 59));
						dt->play();
						lm.getEntities().add(dt);
					}
					break;
				case sf::Keyboard::P:
					if (ui.toggleActive()) {
						lm.pause();
						was_ui_active = true;
					}
					break;
					//{
						//auto action = Game::Action::DO_NOTHING;
						//do {
							//action = ScreenHandler::getInstance().handleScreenEvents(
								//window, PAUSE_SCREEN,
								//PAUSE_SCREEN | PREFERENCES_SCREEN | CONTROLS_SCREEN);
							//if (action == Game::Action::SAVE_GAME) {
								//const auto fname = Game::display_save_dialog();
								//if (fname.length() > 0) {
									////Game::SaveManager::saveGame(fname, *lm);
								//}
								//// TODO: display some confirm screen
							//};
						//} while (action == Game::Action::SAVE_GAME);
						//break;
					//}
				default: break;
				}
			default: break;
			}
		}

		//std::cerr << "# Entities: " << lm.getEntities().size() << std::endl;

#ifndef MULTITHREADED
		if (ui.isActive()) {
			ui.update();
			window.draw(ui);
		} else {
			if (was_ui_active) {
				lm.resume();
				was_ui_active = false;
			}

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
#endif

		// Garbage-collect sounds
		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
	}

	// Perform cleanup
	mm.stop();

	return Game::exitCode;
}
