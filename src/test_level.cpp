#include "Level.hpp"
#include "LevelSet.hpp"
#include <SFML/Window.hpp>
#include <memory>

int main() {
	Game::init();
	sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "test level");
	Game::LevelSet ls("levels.json");
	std::unique_ptr<Game::Level> level(ls.getLevel(1));

	Game::Texture texture(nullptr, Game::getAsset("graphics", "border.png"));
	sf::Sprite sprite;
	sprite.setTexture(*texture.getTexture());
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			window.close();
		}
		}

		window.clear();
		window.draw(*level.get());
		//window.draw(sprite);
		window.display();
	}
}
