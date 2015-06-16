#pragma once

#include <string>
#include <array>
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"
#include "Game.hpp"
#include "Tile.hpp"
#include "utils.hpp"

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
	Matrix<Tile, Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT> tiles;
	/** Time before "Hurry Up" */
	unsigned int time;
	unsigned short tileset = 1;

	void loadTiles();
public:
	Level();
	// XXX: testing constructor
	Level(const std::string& texture_name);
	~Level();

	void draw(sf::RenderTarget& window);

	/** Sets tile at <left>, <top> to <tile> */
	void setTile(unsigned short left, unsigned short top, const Tile& tile);
	void setTileset(unsigned short _tileset) { tileset = _tileset; }
	void setTime(unsigned int _time) { time = _time; }

	Tile getTile(unsigned short left, unsigned short top) const { return tiles[top][left]; }
	unsigned int getTime() const { return time; }
	unsigned short getTileset() const { return tileset; }
};

}
