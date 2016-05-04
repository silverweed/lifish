#include "Level.hpp"
#include "LevelSet.hpp"
#include "Bomb.hpp"
#include "Coin.hpp"
#include "BossExplosion.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "TransparentWall.hpp"
#include "Enemy.hpp"
#include "EntityGroup.hpp"
#include "Teleport.hpp"
#include "Shooting.hpp"
#include <SFML/Window.hpp>
#include <memory>
#include <algorithm>
#include "input.hpp"
#include "Flash.hpp"

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Game::Enemy enemy(sf::Vector2f(32, 32), 1, 1, { .type = Game::AttackType::CONTACT, .damage = 1 });
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
	
	enemy.setMorphed(true);
	enemy.get<Game::AxisMoving>()->setDirection(Game::Direction::DOWN);

	sf::Clock turnClock;

	while (window.isOpen()) {
		std::cerr << "Entities: " << entities.size() << std::endl;

		sf::Event event;
		
		//enemy.get<Game::Shooting>()->shoot();
		if (turnClock.getElapsedTime().asSeconds() > 1) {
			enemy.get<Game::AxisMoving>()->turn(1, false);
			turnClock.restart();
			entities.add(new Game::Flash(sf::Vector2f(300, 300)));
			entities.add(new Game::BossExplosion(sf::Vector2f(400, 200)));
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
		window.display();
	}

	//Game::stopMusic();
}
