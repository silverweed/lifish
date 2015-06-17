#include "Level.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <iostream>
#include <sstream>

using Game::Level;
using Game::pwd;
using Game::DIRSEP;

Level::Level() {}

Level::Level(const std::string& texture_name) {
	initialized = _loadTexture(texture_name);
}

bool Level::_loadTexture(const std::string& texture_name) {
	// Load background texture
	bool loaded = false;
	// Check if image is in cache
	sf::Image *img = Game::cache.getTexture(texture_name);
	if (img != nullptr) {
		if (!bgTexture.loadFromImage(*img)) 
			std::cerr << "[Level.cpp] Error: couldn't load texture " << texture_name << " from memory!" << std::endl;
		else {
			loaded = true;
			std::clog << "[Level.cpp] loaded texture " << texture_name << " from memory." << std::endl;
		}
	}
	// Load from file and update the cache
	if (!loaded) {
		sf::Image *img = new sf::Image;
		if (!img->loadFromFile(texture_name))
			std::cerr << "[Level.cpp] Error: couldn't load texture " << texture_name << " from file!" << std::endl;
		else if (bgTexture.loadFromImage(*img)) {
			loaded = true;
			std::clog << "[Level.cpp] loaded texture " << texture_name << " from file." << std::endl;
			Game::cache.putTexture(texture_name, img);
		} else 
			std::cerr << "[Level.cpp] Error: couldn't load texture " << texture_name << " from image!" << std::endl;
	}
	if (loaded) {
		std::clog << "[Level.cpp] Texture " << texture_name << " loaded correctly. Loading tiles..." << std::endl;
		_loadTiles();
		return true;
	}
	return false;
}

void Level::_loadTiles() {
	for (unsigned short i = 0; i < bgTiles.size(); ++i)
		bgTiles[i].setTexture(bgTexture);

	bgTiles[TILE_REGULAR].setTextureRect(sf::IntRect(3*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setPosition(sf::Vector2f(0, 0));
	bgTiles[TILE_UPPER].setTextureRect(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), 0));
	bgTiles[TILE_RIGHT].setTextureRect(sf::IntRect(3*TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE, TILE_SIZE, -TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), TILE_SIZE * (LEVEL_HEIGHT+1)));
	bgTiles[TILE_LOWER].setTextureRect(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_LEFT].setTextureRect(sf::IntRect(0, TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_LEFT].setPosition(sf::Vector2f(0, TILE_SIZE * (LEVEL_HEIGHT+1)));
	bgTiles[TILE_LEFT].setTextureRect(sf::IntRect(2*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}

Level::~Level() {}

bool Level::init() {
	// Load the appropriate tileset from assets
	std::stringstream assetspath;
	assetspath << pwd << DIRSEP << ".." << DIRSEP << "assets";
	std::stringstream texturename;
	texturename << assetspath.str() << DIRSEP << "textures" << DIRSEP << "tileset" << tileset << ".png";
	initialized = _loadTexture(texturename.str());
	return initialized;
}

void Level::draw(sf::RenderTarget& window) {
	window.draw(bgTiles[TILE_UPPER_LEFT]);
	for (unsigned short i = 1; i < LEVEL_WIDTH + 1; ++i) {
		bgTiles[TILE_UPPER].setPosition(sf::Vector2f(i * TILE_SIZE, 0));
		window.draw(bgTiles[TILE_UPPER]);
	}
	window.draw(bgTiles[TILE_UPPER_RIGHT]);
	for (unsigned short i = 1; i < LEVEL_HEIGHT + 1; ++i) {
		bgTiles[TILE_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), i * TILE_SIZE));
		window.draw(bgTiles[TILE_RIGHT]);
	}
	window.draw(bgTiles[TILE_LOWER_RIGHT]);
	for (unsigned short i = 1; i < LEVEL_WIDTH + 1; ++i) {
		bgTiles[TILE_LOWER].setPosition(sf::Vector2f(i * TILE_SIZE, TILE_SIZE * (LEVEL_HEIGHT+1)));
		window.draw(bgTiles[TILE_LOWER]);
	}
	window.draw(bgTiles[TILE_LOWER_LEFT]);
	for (unsigned short i = 1; i < LEVEL_HEIGHT + 1; ++i) {
		bgTiles[TILE_LEFT].setPosition(sf::Vector2f(0, i * TILE_SIZE));
		window.draw(bgTiles[TILE_LEFT]);
	}
	for (unsigned short i = 1; i < LEVEL_WIDTH + 1; ++i) {
		for (unsigned short j = 1; j < LEVEL_HEIGHT + 1; ++j) {
			sf::Sprite& sprite = bgTiles[TILE_REGULAR];
			switch (tiles[j][i].getType()) {
			using T = Tile::Type;
			case T::EMPTY:
				break;
			case T::BREAKABLE:
				// TODO
				break;
			case T::UNBREAKABLE:
				// TODO
				break;
			default:
				sprite = *tiles[j][i].getSprite();
			}
			sprite.setPosition(sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE));
			window.draw(sprite);
		}
	}
}

void Level::setTile(unsigned short left, unsigned short top, const Tile& tile) {
	if (left > LEVEL_WIDTH-1)
		left = LEVEL_WIDTH - 1;
	if (top > LEVEL_HEIGHT-1)
		top = LEVEL_HEIGHT - 1;

	tiles[top][left] = tile;
}

bool Level::setTilemap(const std::string& tilemap) {
	unsigned short x = 0, y = 0;
	using T = Tile::Type;
	for (unsigned int i = 0; i < tilemap.length(); ++i) {
		switch (tilemap[i]) {
		case T::EMPTY:
		case T::BREAKABLE:
		case T::UNBREAKABLE:
			tiles[y][x].setType(static_cast<T>(tilemap[i]));
			break;
		// TODO: enemies, players, teleporters, etc
		default:
			// invalid tile symbol
			return false;
		}
		if (++x == LEVEL_WIDTH) {
			x = 0;
			if (++y == LEVEL_HEIGHT)
				break;
		}
	}
	return true;
}

void Level::printTilemap() const {
	for (unsigned short i = 0; i < LEVEL_HEIGHT; ++i) {
		for (unsigned short j = 0; j < LEVEL_WIDTH; ++j)
			std::cout << static_cast<char>(tiles[j][i].getType()) << " ";
		std::cout << std::endl;
	}
}
