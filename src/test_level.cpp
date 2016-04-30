#include "Level.hpp"
#include "LevelSet.hpp"
#include "Bomb.hpp"
#include "Coin.hpp"
#include "Enemy.hpp"
#include "EntityGroup.hpp"
#include <SFML/Window.hpp>
#include <memory>
#include <algorithm>
#include "input.hpp"

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "test level");

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Game::Enemy enemy(sf::Vector2f(32, 32), 1);
	Game::Bomb bomb(sf::Vector2f(64, 64), nullptr);
	Game::Coin coin(sf::Vector2f(96, 96));
	Game::Player player(sf::Vector2f(128, 128), 1);
	
	entities.add(&enemy);
	entities.add(&bomb);
	entities.add(&coin);
	entities.add(&player);

	level->setOrigin(sf::Vector2f(-200, 0));
	entities.setOrigin(sf::Vector2f(-200, 0));

	std::cerr << "Entities: " << entities.size() << std::endl;

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	std::array<Game::Player*, 1> players;
	players[0] = &player;

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
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
