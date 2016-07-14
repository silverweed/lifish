#include <SFML/Window.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include "../src/Level.hpp"
#include "../src/LevelLoader.hpp"
#include "../src/dialogs.hpp"
#include "../src/LevelManager.hpp"
#include "../src/LevelSet.hpp"
#include "../src/Bomb.hpp"
#include "../src/Coin.hpp"
#include "../src/Controllable.hpp"
#include "../src/BossExplosion.hpp"
#include "../src/FixedWall.hpp"
#include "../src/BreakableWall.hpp"
#include "../src/TransparentWall.hpp"
#include "../src/Enemy.hpp"
#include "../src/EntityGroup.hpp"
#include "../src/Teleport.hpp"
#include "../src/Shooting.hpp"
#include "../src/Player.hpp"
#include "../src/Flash.hpp"
#include "../src/Options.hpp"
#include "../src/Explosion.hpp"
#include "../src/HomeScreen.hpp"
#include "../src/ScreenHandler.hpp"
#include "../src/Points.hpp"
#include "../src/CollisionDetector.hpp"
#include "../src/SidePanel.hpp"
#include "../src/DroppingText.hpp"
#include <thread>

#ifdef MULTITHREADED
#	ifdef SFML_SYSTEM_LINUX
#		include <X11/Xlib.h>
#	endif
#endif

using namespace Game;

#ifdef MULTITHREADED
static void rendering_loop(sf::RenderWindow& window, const Game::LevelManager& lm, const Game::SidePanel& sidePanel) {
	while (window.isOpen()) {
		window.clear();
		window.draw(lm);
		window.draw(sidePanel);
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
	Game::init();
	if (levelSetName.length() < 1)
		levelSetName = std::string(Game::pwd) + Game::DIRSEP + std::string("levels.json");
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");
	window.setVerticalSyncEnabled(true);
	window.setJoystickThreshold(Game::JOYSTICK_INPUT_THRESHOLD);
	Game::options.showFPS = true;

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

	Game::EntityGroup& entities = lm.getEntities();
	
#if 0
	// XXX For now, manually position players
	players[0]->setPosition(sf::Vector2f(128, 128));
	players[1]->setPosition(sf::Vector2f(128, 128+32));

	// XXX For now, we manually fill the entities

	Attack atype;
	atype.type = AttackType::SIMPLE;
	atype.damage = 1;
	atype.id = 1;
	atype.speed = 1;
	Enemy *enemy = new Enemy(sf::Vector2f(32*6, 32*8), 1, 1, atype);
	Bomb *bomb = new Bomb(sf::Vector2f(64, 64), nullptr);
	Coin *coin = new Coin(sf::Vector2f(96, 96));
	Teleport *teleport = new Teleport(sf::Vector2f(256, 224));
	Teleport *teleport2 = new Teleport(sf::Vector2f(32*11, 320));
	teleport->linkTo(teleport2);
	teleport2->linkTo(teleport);
	FixedWall *wall1 = new FixedWall(sf::Vector2f(32, 64), 5);
	BreakableWall *wall2 = new BreakableWall(sf::Vector2f(32, 96), 3);
	TransparentWall *wall3 = new TransparentWall(sf::Vector2f(32, 128));
	
	entities.add(bomb);
	entities.add(coin);
	entities.add(teleport);
	entities.add(teleport2);
	entities.add(wall1);
	entities.add(wall2);
	entities.add(wall3);
	entities.add(enemy);
	//for (int i = 0; i < 64; ++i) {
		//entities.add(new BreakableWall(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, int(i/Game::LEVEL_WIDTH)*32.0), 2));
		//entities.add(new BreakableWall(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, (Game::LEVEL_HEIGHT-int(i/Game::LEVEL_WIDTH))*32.0), 2));
		//entities.add(new Enemy(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, int(i/Game::LEVEL_WIDTH)*32.0), 1, 1, atype));
	//}
#endif

	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);
	lm.setOrigin(origin);

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	Game::HomeScreen& screen = HomeScreen::getInstance();
	Game::ScreenHandler::getInstance().setOrigin(origin);
	bool drawScreen = false;
	//enemy->get<Game::AxisMoving>()->setDirection(Game::Direction::DOWN);

	sf::Clock turnClock;
	sf::Clock shootClock;

	bool spawned = false;
	int cycle = 0;
	//bool removed = false;
	bool paused = false;

	lm.get<Game::LevelTime>()->resume();
	
#ifdef MULTITHREADED
	window.setActive(false);
	std::thread rendering_thread(rendering_loop, std::ref(window), std::cref(lm), std::cref(sidePanel));
#endif

	while (window.isOpen()) {
		sf::Event event;
		
		//enemy.get<Game::Shooting>()->shoot();
		if (shootClock.getElapsedTime().asMilliseconds() > 200) {
			//float x = float(rand())/RAND_MAX, y = float(rand())/RAND_MAX;
			//float xx = float(rand())/RAND_MAX, yy = float(rand())/RAND_MAX;
			//entities.add(new Game::BossExplosion(sf::Vector2f(
						//Game::LEVEL_WIDTH * Game::TILE_SIZE * xx,
						//Game::LEVEL_HEIGHT * Game::TILE_SIZE * yy)));
			//auto e = new Game::Explosion(sf::Vector2f(
						//Game::LEVEL_WIDTH * Game::TILE_SIZE * x,
						//Game::LEVEL_HEIGHT * Game::TILE_SIZE * y), 2, player);
			//e->propagate(lm);
			//entities.add(e);
			//entities.add(enemy->get<Game::Shooting>()->shoot());
			shootClock.restart();
		}

		//if (turnClock.getElapsedTime().asSeconds() > 1) {
			//if (enemy->isAligned()) {
				//enemy->get<Game::AxisMoving>()->turn(1, false);
				//turnClock.restart();
				//Explosion *expl = new Explosion(sf::Vector2f(7 * 32, 3 * 32), 2, players[0]);
				//expl->propagate(&lm);
				//entities.add(expl);
				//if (rand() < RAND_MAX/2)
					//entities.add(enemy->get<Game::Shooting>()->shoot());
				//if (!spawned) {
					//Points *pts = new Points(sf::Vector2f(320, 12*32), "100");
					//entities.add(pts);
					//spawned = true;
				//}
			//}
			//entities.add(new Game::Flash(sf::Vector2f(300, 300)));
		//}

		//if (!removed && cycle > 100) {
			//entities.remove(wall1); 
			//removed = true;
		//}

		//std::cerr << "Entities: " << entities.size() << std::endl;

		// Event loop
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Q:
					window.close();
					break;
				case sf::Keyboard::M:
					//enemy->setMorphed(!enemy->isMorphed());
					break;
				case sf::Keyboard::Add:
					lvnum = level->getInfo().levelnum + 1;
					if (lvnum > ls.getLevelsNum())
						lvnum = 1;
					level.reset(ls.getLevel(lvnum));
					level->setOrigin(origin);
					LevelLoader::load(*level.get(), lm);
					break;
				case sf::Keyboard::Subtract:
					lvnum = level->getInfo().levelnum - 1;
					if (lvnum < 1) 
						lvnum = ls.getLevelsNum();
					level.reset(ls.getLevel(lvnum));
					level->setOrigin(origin);
					LevelLoader::load(*level.get(), lm);
					break;
				case sf::Keyboard::L:
					if (paused) {
						std::cerr << "\n==== CYCLE " << cycle << " ====" << std::endl;
						lm.update();
					} else
						paused = true;
					break;
				case sf::Keyboard::K:
					paused = !paused;
					break;
				case sf::Keyboard::T:
					{
						auto dt = new Game::DroppingText(
								Game::getAsset("test", "gameover.png"),
								sf::Vector2i(311, 59));
						dt->play();
						entities.add(dt);
					}
					break;
				case sf::Keyboard::P:
					{
						auto action = Game::Action::DO_NOTHING;
						do {
							action = ScreenHandler::getInstance().handleScreenEvents(
								window, PAUSE_SCREEN,
								PAUSE_SCREEN | PREFERENCES_SCREEN | CONTROLS_SCREEN);
							if (action == Game::Action::SAVE_GAME) {
								const auto fname = Game::display_save_dialog();
								if (fname.length() > 0) {
									//Game::SaveManager::saveGame(fname, *lm);
								}
								// TODO: display some confirm screen
							};
						} while (action == Game::Action::SAVE_GAME);
						break;
					}
				default: break;
				}
			default: break;
			}
		}


		// FIXME
		//for (auto player : players) {
			//if (player->get<Game::Controllable>()->hasFocus() 
				//&& player->isAligned() 
				//&& sf::Keyboard::isKeyPressed(
					//Game::Controls::players[player->getInfo().id-1][Game::Controls::CTRL_BOMB]))
			//{
				//entities.add(new Game::Bomb(player->getPosition(), *player));	
			//}
		//}

		// Update level
		if (!paused)
			lm.update();

		sidePanel.update();

#ifndef MULTITHREADED
		// Draw everything
		window.clear();
		window.draw(lm);
		window.draw(sidePanel);
		Game::maybeShowFPS(window);
		window.display();
#endif

		// Garbage-collect sounds
		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
	}

	//Game::stopMusic();
}
