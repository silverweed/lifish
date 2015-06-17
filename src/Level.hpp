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
	/** The background texture */
	sf::Texture bgTexture;
	/** The sprites for the background tiles */
	std::array<sf::Sprite, 9> bgTiles;
	/** The content of the tiles of this level */
	Matrix<Tile, Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT> tiles;
	/** Time before "Hurry Up" */
	unsigned int time;
	/** The tileset to use */
	unsigned short tileset = 1;

	/** Loads the content of bgTiles (bgTexture must already be set) */
	void _loadTiles();
	/** Loads the texture from the file <texture_name> (or from memory, if
	 *  texture has already been cached)
	 */
	bool _loadTexture(const std::string& texture_name);
	/** Whether this level has been initialized or not */
	bool initialized = false;
public:
	/** Constructs a level without a specified time and tileset. init() must
	 *  be called before using this level.
	 */
	Level();
	// XXX: testing constructor
	Level(const std::string& texture_name);
	virtual ~Level();

	/** Loads the appropriate bgTexture, fills the bgTiles and makes this level
	 *  usable after either constructing it without parameters or after changing
	 *  them. Returns false if there were errors, true otherwise.
	 */
	bool init();

	bool isInitialized() const { return initialized; }

	void draw(sf::RenderTarget& window);

	/** Sets tile at <left>, <top> to <tile> */
	void setTile(unsigned short left, unsigned short top, const Tile& tile);
	void setTileset(unsigned short _tileset) { tileset = _tileset; }
	void setTime(unsigned int _time) { time = _time; }
	/** Given a string representation of the level, loads it into <tiles> */
	bool setTilemap(const std::string& tilemap);

	Tile getTile(unsigned short left, unsigned short top) const { return tiles[top][left]; }
	unsigned int getTime() const { return time; }
	unsigned short getTileset() const { return tileset; }

	void printTilemap() const;
};

}
