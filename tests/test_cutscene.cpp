#include "Cutscene.hpp"
#include "core.hpp"
#include "Options.hpp"

#define LAYER(s, d) std::make_tuple(lif::getAsset("test/cutscenes", s), d)
#define SUB(s, t) std::make_pair(s, sf::seconds(t))

int main() {
	lif::init();

	lif::Cutscene cut(sf::Vector2u(640, 480), {
		LAYER("bands.png", 0),
		LAYER("1_-1.png", 4),
		LAYER("1_0.png", 5),
		LAYER("1_0_5.png", 8),
		LAYER("1_1.png", 10),
		LAYER("1_2.png", 15),
		LAYER("1_3.png", 30),
	}, {
		SUB("It was a dark and gloomy night...", 3),
		SUB("And I felt the hatred of ten thousand years...", 3),
		SUB("Yet...", 2),
		SUB("My vengeance was just at its commencement...", 4),
		SUB("My enemies would soon suffer...", 2.5),
		SUB("Like I am.", 5),
	});

	sf::RenderWindow window(sf::VideoMode(640, 480), "test_cutscene");
	window.setFramerateLimit(120);
	lif::options.vsync = true;
	lif::options.showFPS = false;

	lif::WindowContext *cur_context = &cut;

	while (window.isOpen() && !lif::terminated) {
		cur_context->handleEvents(window);

		cur_context->update();

		window.clear();
		window.draw(*cur_context);
		lif::maybeShowFPS(window);
		window.display();
	}

	if (window.isOpen()) window.close();
}
