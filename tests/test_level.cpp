#include <SFML/Window.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include "../src/Level.hpp"
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

using namespace Game;

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");
	window.setVerticalSyncEnabled(true);
	window.setJoystickThreshold(Game::JOYSTICK_INPUT_THRESHOLD);
	Game::options.showFPS = true;

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Attack atype;
	atype.type = AttackType::SIMPLE;
	atype.damage = 1;
	atype.id = 1;
	atype.speed = 1;
	Enemy *enemy = new Enemy(sf::Vector2f(32*6, 32*8), 1, 1, atype);
	Player *player = new Player(sf::Vector2f(128, 128), 1);
	Player *player2 = new Player(sf::Vector2f(128, 128 + 32), 2);
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
	entities.add(player);
	entities.add(player2);
	//for (int i = 0; i < 64; ++i) {
		//entities.add(new BreakableWall(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, int(i/Game::LEVEL_WIDTH)*32.0), 2));
		//entities.add(new BreakableWall(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, (Game::LEVEL_HEIGHT-int(i/Game::LEVEL_WIDTH))*32.0), 2));
		//entities.add(new Enemy(sf::Vector2f((i%Game::LEVEL_WIDTH)*32, int(i/Game::LEVEL_WIDTH)*32.0), 1, 1, atype));
	//}

	level->setOrigin(sf::Vector2f(-200, 0));
	entities.setOrigin(sf::Vector2f(-200, 0));

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	Game::HomeScreen& screen = HomeScreen::getInstance();
	Game::ScreenHandler::getInstance().setOrigin(sf::Vector2f(-200, 0));
	bool drawScreen = false;

	std::array<Game::Player*, 2> players;
	players[0] = player;
	players[1] = player2;
	
	player->get<Game::Controllable>()->setWindow(&window);
	player2->get<Game::Controllable>()->setWindow(&window);
	enemy->get<Game::AxisMoving>()->setDirection(Game::Direction::DOWN);

	sf::Clock turnClock;
	sf::Clock shootClock;

	auto lm = new Game::LevelManager;

	Game::CollisionDetector cd(entities);

	bool spawned = false;
	int cycle = 0;
	//bool removed = false;

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
			entities.add(enemy->get<Game::Shooting>()->shoot());
			shootClock.restart();
		}

		if (turnClock.getElapsedTime().asSeconds() > 1) {
			if (enemy->isAligned()) {
				enemy->get<Game::AxisMoving>()->turn(1, false);
				turnClock.restart();
				Explosion *expl = new Explosion(sf::Vector2f(7 * 32, 3 * 32), 2, player);
				expl->propagate(lm);
				//entities.add(expl);
				//if (rand() < RAND_MAX/2)
					//entities.add(enemy->get<Game::Shooting>()->shoot());
				if (!spawned) {
					Points *pts = new Points(sf::Vector2f(320, 12*32), "100");
					entities.add(pts);
					spawned = true;
				}
			}
			//entities.add(new Game::Flash(sf::Vector2f(300, 300)));
		}

		//if (!removed && cycle > 100) {
			//entities.remove(wall1); 
			//removed = true;
		//}

		// Event loop
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::M:
					enemy->setMorphed(!enemy->isMorphed());
					break;
				case sf::Keyboard::S:
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						drawScreen = !drawScreen;
					break;
				case sf::Keyboard::P:
					{
						auto action = Game::Action::DO_NOTHING;
						do {
							action = ScreenHandler::getInstance().handleScreenEvents(window, PAUSE_SCREEN,
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

		// Update entities and their components
		entities.updateAll();

		// Update collisions
		cd.update();

		// Draw everything
		window.clear();
		if (!drawScreen) {
			window.draw(*level.get());
			entities.apply([&window] (Game::Entity *e) {
				auto d = e->get<Game::Drawable>();
				if (d != nullptr)
					window.draw(*d);
			});
		} else {
			window.draw(Game::HomeScreen::getInstance());
		}
		Game::maybeShowFPS(window);
		window.display();

		if (++cycle >= Game::GameCache::SOUNDS_GC_DELAY) {
			cycle = 0;
			Game::cache.gcSounds();
		}
	}

	delete lm;

	//Game::stopMusic();
}
