#pragma once

#include <string>
#include <array>
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"

namespace Game {

class Level : public Game::Drawable {
	enum : unsigned short {
		TILE_REGULAR = 0,
		TILE_UPPER_LEFT = 1,
		TILE_UPPER_RIGHT = 2,
		TILE_LOWER_LEFT = 3,
		TILE_LOWER_RIGHT = 4,
		TILE_UPPER = 5,
		TILE_LOWER = 6,
		TILE_LEFT = 7,
		TILE_RIGHT = 8,
	};
	sf::Texture bgTexture;
	std::array<sf::Sprite, 9> bgTiles;
public:
	// XXX: testing constructor
	Level(const std::string& texture_name);
	~Level();

	void draw(sf::RenderTarget& window);
};

}
