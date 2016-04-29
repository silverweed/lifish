#include "Level.hpp"
#include "LevelSet.hpp"
#include "Bomb.hpp"
#include "Coin.hpp"
#include "EntityGroup.hpp"
#include <SFML/Window.hpp>
#include <memory>
#include <algorithm>

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "test level");

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::EntityGroup entities;

	Game::Entity entity;
	entity.setPosition(sf::Vector2f(32, 32));
	Game::Animated anim(&entity, Game::getAsset("graphics", "enemy1.png"));
	anim.addAnimation("move", {
		sf::IntRect(0, 0, 32, 32),
		sf::IntRect(32, 0, 32, 32),
		sf::IntRect(64, 0, 32, 32),
		sf::IntRect(96, 0, 32, 32) 
	}, true);
	entity.addComponent(&anim);

	Game::Bomb bomb(sf::Vector2f(64, 64), nullptr);
	Game::Coin coin(sf::Vector2f(96, 96));
	
	entities.add(&entity);
	entities.add(&bomb);
	entities.add(&coin);

	entities.setOrigin(sf::Vector2f(-200, 0));

	std::cerr << "Entities: " << entities.size() << std::endl;

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

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
