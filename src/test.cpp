#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");

	// Optional: use OpenGL / VSync
	//glEnable(GL_TEXTURE_2D);
	window.setVerticalSyncEnabled(true);

	sf::CircleShape shape(100.f);
	//shape.setFillColor(sf::Color::Green);

	//sf::Clock clock; // start the clock
	//sf::Time elapsed;

	sf::Texture texture;
	if (!texture.loadFromFile("image.png"))
		std::cerr << "Error loading texture!" << std::endl;
	texture.setSmooth(true);
	sf::Sprite sprite;
	sprite.setTexture(texture);

	sf::Font font;
	font.loadFromFile("FreeMono.ttf");
	sf::Text text;
	text.setFont(font);
	text.setString(L"יטאח faggot");
	text.setCharacterSize(24); // px
	text.setColor(sf::Color::Blue);
	text.setStyle(sf::Text::Bold);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				std::cout << "\nPressed: " << event.key.code << std::endl;
				break;
/*			case sf::Event::MouseMoved:
				std::cout << "\nMouse moved to: " << event.mouseMove.x <<
					", " << event.mouseMove.y << std::endl;
				break;*/
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
				case sf::Mouse::Button::Left:
					sprite.setColor(sf::Color(255, 255, 255, 255));
					break;
				case sf::Mouse::Button::Right:
					sprite.setColor(sf::Color(0, 255, 255, 255));
					break;
				case sf::Mouse::Button::Middle:
					sprite.setColor(sf::Color(255, 255, 255, 155));
					break;
				case sf::Mouse::Button::XButton1:
					sprite.setColor(sf::Color(255, 0, 255, 255));
					break;
				case sf::Mouse::Button::XButton2:
					sprite.setColor(sf::Color(255, 0, 0, 128));
					break;
				}
				break;
			}	
		}

		window.clear();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			shape.setFillColor(sf::Color::Red);
		else
			shape.setFillColor(sf::Color::Green);
		window.draw(shape);
		window.draw(sprite);
		window.draw(text);
		window.display();
		//elapsed = clock.getElapsedTime();
	}
	return 0;
}
