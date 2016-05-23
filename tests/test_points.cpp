#include <SFML/Window.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include "../src/Level.hpp"
#include "../src/AxisMoving.hpp"
#include "../src/LevelSet.hpp"
#include "../src/EntityGroup.hpp"
#include "../src/Options.hpp"
#include "../src/Explosion.hpp"
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
	entities.setOrigin(sf::Vector2f(-200, 0));
	
	level->setOrigin(sf::Vector2f(-200, 0));

	sf::Clock clock;

	auto lm = new Game::LevelManager;

	bool spawned = false;
	
	//std::list<Points*> ptss;
	
	while (window.isOpen()) {
		sf::Event event;
		if (clock.getElapsedTime().asSeconds() > 1) {
			if (!spawned) {
				Points *pts = new Points(sf::Vector2f(320, 12*32), "100");
				entities.add(pts);
				spawned = true;
			} else {
				entities.add(new Explosion(sf::Vector2f(222, 222), 1));
			}
			clock.restart();
		}

		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			default: break;
			}
		}

		entities.updateAll();

		// Set entities' previous align	
		//entities.apply([] (Game::Entity *e) {
			//auto m = e->get<Game::AxisMoving>();
			//if (m != nullptr && e->isAligned())
				//m->setPrevAlign(Game::tile(e->getPosition()));
		//});

		// Draw everything
		window.clear();
		window.draw(*level.get());
		entities.apply([&window] (Game::Entity *e) {
			auto d = e->get<Game::Drawable>();
			if (d != nullptr)
				window.draw(*d);
		});
		//for (auto pts : ptss) {
			//pts->update();
			//window.draw(*pts->get<Game::Drawable>());
		//}
		Game::maybeShowFPS(window);
		window.display();
	}

	delete lm;
}
