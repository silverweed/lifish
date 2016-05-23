#include <SFML/Window.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include "../src/Level.hpp"
#include "../src/LevelSet.hpp"
#include "../src/Bomb.hpp"
#include "../src/Coin.hpp"
#include "../src/BossExplosion.hpp"
#include "../src/FixedWall.hpp"
#include "../src/BreakableWall.hpp"
#include "../src/TransparentWall.hpp"
#include "../src/Enemy.hpp"
#include "../src/EntityGroup.hpp"
#include "../src/Teleport.hpp"
#include "../src/Shooting.hpp"
#include "../src/input.hpp"
#include "../src/Flash.hpp"
#include "../src/Options.hpp"
#include "../src/Explosion.hpp"
#include "../src/HomeScreen.hpp"
#include "../src/ScreenHandler.hpp"
#include "../src/Points.hpp"

using namespace Game;

// MOCK
namespace Game {
class LevelManager {

};
}

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");
	window.setVerticalSyncEnabled(true);
	Game::options.showFPS = true;

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Game::Attack atype;
	atype.type = Game::AttackType::SIMPLE;
	atype.damage = 1;
	atype.id = 1;
	atype.speed = 1;
	Enemy *enemy = new Enemy(sf::Vector2f(32*6, 32*8), 1, 1, atype);
	Player *player = new Player(sf::Vector2f(128, 128), 1);
	Bomb *bomb = new Bomb(sf::Vector2f(64, 64), nullptr);
	Coin *coin = new Coin(sf::Vector2f(96, 96));
	Teleport *teleport = new Teleport(sf::Vector2f(256, 256));
	FixedWall *wall1 = new FixedWall(sf::Vector2f(32, 64), 5);
	BreakableWall *wall2 = new BreakableWall(sf::Vector2f(32, 96), 3);
	TransparentWall *wall3 = new TransparentWall(sf::Vector2f(32, 128));
	
	//entities.add(enemy);
	entities.add(bomb);
	entities.add(coin);
	//entities.add(player);
	entities.add(teleport);
	entities.add(wall1);
	entities.add(wall2);
	entities.add(wall3);

	level->setOrigin(sf::Vector2f(-200, 0));
	entities.setOrigin(sf::Vector2f(-200, 0));

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	Game::HomeScreen& screen = HomeScreen::getInstance();
	screen.setOrigin(sf::Vector2f(-200, 0));
	bool drawScreen = false;

	std::array<Game::Player*, 1> players;
	players[0] = player;
	
	enemy->get<Game::AxisMoving>()->setDirection(Game::Direction::DOWN);

	sf::Clock turnClock;
	sf::Clock shootClock;

	auto lm = new Game::LevelManager;

	bool spawned = false;

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
					drawScreen = !drawScreen;
					break;
				default: break;
				}
			default: break;
			}
		}

		// Fill players' directions according to input
		Game::get_directions(window, players);

		entities.updateAll();

		// Set entities' previous align	
		entities.apply([] (Game::Entity *e) {
			auto m = e->get<Game::AxisMoving>();
			if (m != nullptr && e->isAligned())
				m->setPrevAlign(Game::tile(e->getPosition()));
		});

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
			//switch (Game::ScreenHandler::getInstance().handleScreenEvents(window, 
				//HOME_SCREEN,
				//HOME_SCREEN)) 
			//{
			//case Action::EXIT:
				//drawScreen = false;
			//}
			//window.draw(Game::ScreenHandler::getInstance());
			window.draw(Game::HomeScreen::getInstance());
		}
		Game::maybeShowFPS(window);
		window.display();
	}

	delete lm;

	//Game::stopMusic();
}
