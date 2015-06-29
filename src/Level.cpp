#include "Level.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <iostream>
#include <sstream>

using Game::Level;
using Game::pwd;
using Game::DIRSEP;
using Game::EntityType;

Level::Level(const LevelSet *const _levelSet) : levelSet(_levelSet) {}

bool Level::_loadTexture() {
	// Load background texture
	std::stringstream ss;
	ss << "bg" << tileIDs.bg << ".png";
	if (!Game::cache.loadTexture(bgTexture, Game::getAsset("graphics", ss.str()))) {
		std::cerr << "[Level.cpp] Could not load background texture!" << std::endl;
		return false;
	}
	if (!Game::cache.loadTexture(borderTexture, Game::getAsset("graphics", "border.png"))) {
		std::cerr << "[Level.cpp] Could not load border texture!" << std::endl;
		return false;
	}
	_loadTiles();
	return true;
}

void Level::_loadTiles() {
	// The size of the background tiles
	const unsigned short tsize = 2 * TILE_SIZE;

	bgTiles[TILE_REGULAR].setTexture(bgTexture);
	bgTexture.setRepeated(true);
	bgTiles[TILE_REGULAR].setTextureRect(sf::IntRect(0, 0, tsize, tsize));

	borderTexture.setRepeated(true);
	for (unsigned short i = 1; i < bgTiles.size(); ++i)
		bgTiles[i].setTexture(borderTexture);

	bgTiles[TILE_UPPER].setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	// TILE_UPPER y-mirrored
	bgTiles[TILE_LOWER].setTextureRect(sf::IntRect(0, TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LEFT].setTextureRect(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	// TILE_LEFT x-mirrored
	bgTiles[TILE_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE + TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setTextureRect(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	// TILE_UPPER_LEFT x-mirrored
	bgTiles[TILE_UPPER_RIGHT].setTextureRect(sf::IntRect(2 * TILE_SIZE + TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setTextureRect(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	// TILE_LOWER_RIGHT x-mirrored
	bgTiles[TILE_LOWER_LEFT].setTextureRect(sf::IntRect(3 * TILE_SIZE + TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));

	// Set angles' position once and for all
	bgTiles[TILE_UPPER_LEFT].setPosition(sf::Vector2f(0, 0));
	bgTiles[TILE_UPPER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), 0));
	bgTiles[TILE_LOWER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), TILE_SIZE * (LEVEL_HEIGHT+1)));
	bgTiles[TILE_LOWER_LEFT].setPosition(sf::Vector2f(0, TILE_SIZE * (LEVEL_HEIGHT+1)));
}

bool Level::_loadMusic(const std::string& music_name) {
	if (!musicInput.openFromFile(music_name)) {
		std::cerr << "[Level.cpp] Error: couldn't load music " << music_name << " from file!" << std::endl;
		return false;
	}
	music = new LoopingMusic(musicInput);
	music->setLoopPoints(sf::seconds(track.loopstart), sf::seconds(track.loopend));
	music->setLoop(true);
	std::clog << "[Level.cpp] Loaded music " << music_name << "; loop: (" << music->getLoopBegin().asSeconds()
		<< ", " << music->getLoopEnd().asSeconds() << ")" << std::endl;
	return true;
}

Level::~Level() {
	if (music != nullptr)
		delete music;
	if (levelnumtext != nullptr)
		delete levelnumtext;
}

bool Level::init() {
	initialized = _loadTexture() && _loadMusic(track.name);

	// Load the levelnum text
	levelnumtext = ShadedText::newShadedText(
			Game::getAsset("fonts", LEVELNUM_FONT),
			std::to_string(levelnum),
			sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), 0));
	levelnumtext->setStyle(sf::Text::Bold);

	return initialized;
}

void Level::draw(sf::RenderTarget& window) {
	// The size of the background/border tiles
	const unsigned short tsize = 2 * TILE_SIZE;
	// Border tiles number on x axis
	const unsigned short btn_x = LEVEL_WIDTH * TILE_SIZE / tsize + 1;
	// Border tiles number on y axis
	const unsigned short btn_y = LEVEL_HEIGHT * TILE_SIZE / tsize + 1;

	// Draw the borders
	window.draw(bgTiles[TILE_UPPER_LEFT]);
	for (unsigned short i = 1; i < 2*btn_x; ++i) {
		bgTiles[TILE_UPPER].setPosition(sf::Vector2f(i * TILE_SIZE, 0));
		window.draw(bgTiles[TILE_UPPER]);
	}
	for (unsigned short i = 1; i < 2*btn_y; ++i) {
		bgTiles[TILE_LEFT].setPosition(sf::Vector2f(0, i * TILE_SIZE));
		window.draw(bgTiles[TILE_LEFT]);
	}
	window.draw(bgTiles[TILE_UPPER_RIGHT]);

	// Draw the level background
	for (unsigned short i = 0; i < btn_x; ++i) {
		for (unsigned short j = 0; j < btn_y; ++j) {
			bgTiles[TILE_REGULAR].setPosition(sf::Vector2f(i * tsize + TILE_SIZE, j * tsize + TILE_SIZE));
			window.draw(bgTiles[TILE_REGULAR]);
		}
	}

	for (unsigned short i = 1; i < 2*(btn_y); ++i) {
		bgTiles[TILE_RIGHT].setPosition(sf::Vector2f(2*btn_x * TILE_SIZE, i * TILE_SIZE));
		window.draw(bgTiles[TILE_RIGHT]);
	}

	window.draw(bgTiles[TILE_LOWER_RIGHT]);
	for (unsigned short i = 1; i < 2*btn_x; ++i) {
		bgTiles[TILE_LOWER].setPosition(sf::Vector2f(i * TILE_SIZE, 2*btn_y * TILE_SIZE));
		window.draw(bgTiles[TILE_LOWER]);
	}
	window.draw(bgTiles[TILE_LOWER_LEFT]);

	// Draw the level number
	if (levelnumtext != nullptr)
		levelnumtext->draw(window);
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
		  << "Level " << levelnum << "\n"
		  << "Time: " << time << " s\n"
		  << "Tiles: {\n"
		  << "    bg: " << tileIDs.bg << "\n"
		  << "    border: " << tileIDs.border << "\n"
		  << "    fixed: " << tileIDs.fixed << "\n"
		  << "    breakable: " << tileIDs.breakable << "\n}\n"
		  << "Music: " << track.name << "\n";
	if (levelSet != nullptr) {
		std::cout << "Belongs to: >>>\n";
		levelSet->printInfo();
		std::cout << "<<<\n";
	}
	std::cout << "Tilemap:\n";
	printTilemap();
}
