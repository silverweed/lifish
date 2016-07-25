#include "ControlsScreen.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "Options.hpp"
#include "utils.hpp"

using namespace Game::UI;

int main(int argc, char **argv) {
	sf::RenderWindow window(sf::VideoMode(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), "screen test");
	window.setVerticalSyncEnabled(true);
	
	Game::init();
	Game::options.showFPS = true;
	//Screen screen(argc > 1 ? argv[1] : "home.json", window);

	UI& ui = UI::getInstance();
	ui.load(window, { "home.json", "about.json", "preferences.json" });
	ui.add(new Game::UI::ControlsScreen(window));
	ui.setActive(true);

	while (window.isOpen()) {
		sf::Event event;
		if (ui.isActive())
			ui.handleEvents(window);
		else while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Q:
					window.close();
					break;
				case sf::Keyboard::F:
					Game::options.showFPS = !Game::options.showFPS;
					break;
				default:
					break;
				}
			default:
				break;
			}
		}
	
		ui.update();
		window.clear();
		window.draw(ui);
		Game::maybeShowFPS(window);
		window.display();
	}
}
