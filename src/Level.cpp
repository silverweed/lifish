#include "Level.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <iostream>
#include <sstream>

using Game::Level;
using Game::pwd;
using Game::DIRSEP;
using Game::EntityType;

Level::Level(const LevelSet *_levelSet) : levelSet(_levelSet) {}

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

bool Level::_loadMusic(const std::string& music_name) {
	sf::InputSoundFile input;
	if (!input.openFromFile(music_name)) {
		std::cerr << "[Level.cpp] Error: couldn't load music " << music_name << " from file!" << std::endl;
		return false;
	}
	music = new LoopingMusic(input);
	music->setLoopPoints(sf::seconds(track->getLoopStart()), sf::seconds(track->getLoopEnd()));
	std::clog << "[Level.cpp] Loaded music " << music_name << "; loop: (" << music->getLoopBegin().asSeconds()
		<< ", " << music->getLoopEnd().asSeconds() << ")" << std::endl;
	return true;
}

Level::~Level() {
	if (music != nullptr)
		delete music;
	// don't delete track as it's managed by LevelSet
}

bool Level::init() {
	// Load the appropriate tileset from assets
	std::stringstream assetspath;
	assetspath << pwd << DIRSEP << "assets";
	std::stringstream texturename;
	texturename << assetspath.str() << DIRSEP << "textures" << DIRSEP << "tileset" << tileset << ".png";
	initialized = _loadTexture(texturename.str());
	// Load the music
	initialized &= _loadMusic(track->getName());
	return initialized;
}

void Level::draw(sf::RenderTarget& window) {
	// Draw the borders
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
	// Draw the level background
	for (unsigned short i = 1; i < LEVEL_WIDTH + 1; ++i) {
		for (unsigned short j = 1; j < LEVEL_HEIGHT + 1; ++j) {
			bgTiles[TILE_REGULAR].setPosition(sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE));
			window.draw(bgTiles[TILE_REGULAR]);
		}
	}
}

EntityType Level::getTile(unsigned short left, unsigned short top) const {
	if (left >= LEVEL_WIDTH || top >= LEVEL_HEIGHT) return EntityType::UNKNOWN;
	return tiles[top][left];
}

bool Level::setTilemap(const std::string& tilemap) {
	unsigned short x = 0, y = 0;
	for (unsigned int i = 0; i < tilemap.length(); ++i) {
		EntityType et = Game::entityFromLetter(tilemap[i]);
		if (et == EntityType::UNKNOWN) return false;
		tiles[y][x] = et;
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
		for (unsigned short j = 0; j < LEVEL_WIDTH; ++j) {
			std::cout << tiles[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void Level::printInfo() const {
	std::cout << "Level Info:\n" 
		  << "-----------\n"
		  << "Time: " << time << " s\n"
		  << "Tileset: " << tileset << "\n"
		  << "Music: " << track->getName() << "\n";
	if (levelSet != nullptr) {
		std::cout << "Belongs to: >>>\n";
		levelSet->printInfo();
		std::cout << "<<<\n";
	}
	std::cout << "Tilemap:\n";
	printTilemap();
}
