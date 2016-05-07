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

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");
	Game::options.showFPS = true;

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Game::Attack atype;
	atype.type = Game::AttackType::CONTACT;
	atype.damage = 1;
	Game::Enemy enemy(sf::Vector2f(32*6, 32*8), 1, 1, atype);
	Game::Bomb bomb(sf::Vector2f(64, 64), nullptr);
	Game::Coin coin(sf::Vector2f(96, 96));
	Game::Player player(sf::Vector2f(128, 128), 1);
	Game::Teleport teleport(sf::Vector2f(256, 256));
	Game::FixedWall wall1(sf::Vector2f(32, 64), 5);
	Game::BreakableWall wall2(sf::Vector2f(32, 96), 3);
	Game::TransparentWall wall3(sf::Vector2f(32, 128));
	
	entities.add(&enemy);
	entities.add(&bomb);
	entities.add(&coin);
	entities.add(&player);
	entities.add(&teleport);
	entities.add(&wall1);
	entities.add(&wall2);
	entities.add(&wall3);

	level->setOrigin(sf::Vector2f(-200, 0));
	entities.setOrigin(sf::Vector2f(-200, 0));

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	std::array<Game::Player*, 1> players;
	players[0] = &player;
	
	enemy.get<Game::AxisMoving>()->setDirection(Game::Direction::DOWN);

	sf::Clock turnClock;

	while (window.isOpen()) {
		std::cerr << "Entities: " << entities.size() << std::endl;
		std::cerr << "Bullets: " << entities.size<Game::Bullet>() << std::endl;

		sf::Event event;
		
		//enemy.get<Game::Shooting>()->shoot();
	/*	if (turnClock.getElapsedTime().asMilliseconds() > 100) {
			float x = float(rand())/RAND_MAX, y = float(rand())/RAND_MAX;
			entities.add(new Game::BossExplosion(sf::Vector2f(
						Game::LEVEL_WIDTH * Game::TILE_SIZE * x + 200,
						Game::LEVEL_HEIGHT * Game::TILE_SIZE * y)));
		}*/

		if (turnClock.getElapsedTime().asSeconds() > 1) {
			if (enemy.isAligned()) {
				enemy.get<Game::AxisMoving>()->turn(1, false);
				turnClock.restart();
				if (rand() < RAND_MAX/2)
					entities.add(enemy.get<Game::Shooting>()->shoot());
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
					enemy.setMorphed(!enemy.isMorphed());
					break;
				default: break;
				}
			default: break;
			}
		}

		Game::get_directions(window, players);

		entities.updateAll();
		window.clear();
		window.draw(*level.get());
		entities.apply([&window] (Game::Entity* e) {
			auto d = e->get<Game::Drawable>();
			if (d != nullptr)
				window.draw(*d);
		});
		Game::maybeShowFPS(window);
		window.display();
	}

	//Game::stopMusic();
}
