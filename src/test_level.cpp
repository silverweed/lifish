#include "Level.hpp"
#include "LevelSet.hpp"
#include "Bomb.hpp"
#include <SFML/Window.hpp>
#include <memory>

int main() {
	Game::init();
	
	sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "test level");

	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::Entity entity;
	entity.setPosition(sf::Vector2f(32, 32));
	Game::Animated anim(&entity, Game::getAsset("graphics", "enemy1.png"));
	anim.addAnimation("move", {
		sf::IntRect(0, 0, 32, 32),
		sf::IntRect(32, 0, 32, 32),
		sf::IntRect(64, 0, 32, 32),
		sf::IntRect(96, 0, 32, 32) 
	}, true);

	Game::Bomb bomb(sf::Vector2f(64, 64), nullptr);

	Game::music = level->get<Game::Music>()->getMusic();
	//Game::playMusic();

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
			}
		}

		anim.update();
		bomb.update();

		window.clear();
		window.draw(*level.get());
		window.draw(anim);
		window.draw(*bomb.get<Game::Drawable>());
		window.display();
	}

	//Game::stopMusic();
}
