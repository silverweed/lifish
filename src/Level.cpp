#include "Level.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>

using Game::Level;
using Game::pwd;
using Game::DIRSEP;
using Game::EntityType;

Level::Level(const Game::LevelSet *const _levelSet) : levelSet(_levelSet) {}

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
	bgTiles[TILE_REGULAR].setTexture(bgTexture);
	bgTexture.setRepeated(true);
	bgTexture.setSmooth(true);
	bgTiles[TILE_REGULAR].setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));

	for (unsigned short i = 1; i < bgTiles.size(); ++i)
		bgTiles[i].setTexture(borderTexture);

	const unsigned short b = (tileIDs.border-1) * TILE_SIZE;

	bgTiles[TILE_UPPER].setTextureRect(sf::IntRect(0, b, TILE_SIZE, TILE_SIZE));
	// TILE_UPPER y-mirrored
	bgTiles[TILE_LOWER].setTextureRect(sf::IntRect(0, b + TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LEFT].setTextureRect(sf::IntRect(TILE_SIZE, b, TILE_SIZE, TILE_SIZE));
	// TILE_LEFT x-mirrored
	bgTiles[TILE_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE + TILE_SIZE, b, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setTextureRect(sf::IntRect(2 * TILE_SIZE, b, TILE_SIZE, TILE_SIZE));
	// TILE_UPPER_LEFT x-mirrored
	bgTiles[TILE_UPPER_RIGHT].setTextureRect(sf::IntRect(2 * TILE_SIZE + TILE_SIZE, b, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setTextureRect(sf::IntRect(3 * TILE_SIZE, b, TILE_SIZE, TILE_SIZE));
	// TILE_LOWER_RIGHT x-mirrored
	bgTiles[TILE_LOWER_LEFT].setTextureRect(sf::IntRect(3 * TILE_SIZE + TILE_SIZE, b, -TILE_SIZE, TILE_SIZE));

	// Set angles' position once and for all
	bgTiles[TILE_UPPER_LEFT].setPosition(0, 0);
	bgTiles[TILE_UPPER_RIGHT].setPosition(TILE_SIZE * (LEVEL_WIDTH+1), 0);
	bgTiles[TILE_LOWER_RIGHT].setPosition(TILE_SIZE * (LEVEL_WIDTH+1), TILE_SIZE * (LEVEL_HEIGHT+1));
	bgTiles[TILE_LOWER_LEFT].setPosition(0, TILE_SIZE * (LEVEL_HEIGHT+1));
}

bool Level::_loadMusic(const std::string& music_name) {
	if (!musicInput.openFromFile(music_name)) {
		std::cerr << "[Level.cpp] Error: couldn't load music " << music_name << " from file!" << std::endl;
		return false;
	}
	music = new LoopingMusic(musicInput);
	music->setLoopPoints(sf::seconds(track.loopstart), sf::seconds(track.loopend));
	music->setLoop(true);
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
	levelnumtext = new Game::ShadedText(
			Game::getAsset("fonts", Game::Fonts::LEVELNUM),
			Game::to_string(levelnum),
			sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), 0));
	levelnumtext->setStyle(sf::Text::Bold);
	levelnumtext->setCharacterSize(20);

	return initialized;
}

void Level::setOrigin(const sf::Vector2f& origin) {
	for (unsigned short i = 0; i < bgTiles.size(); ++i)
		bgTiles[i].setOrigin(origin);
	levelnumtext->setOrigin(origin);
}

void Level::draw(sf::RenderTarget& window) {
	// Border tiles number on x axis
	const unsigned short btn_x = LEVEL_WIDTH + 1;
	// Border tiles number on y axis
	const unsigned short btn_y = LEVEL_HEIGHT + 1;

	// Draw the level background
	for (unsigned short i = 0; i < btn_x + 1; ++i) {
		for (unsigned short j = 0; j < btn_y + 1; ++j) {
			bgTiles[TILE_REGULAR].setPosition(i * TILE_SIZE, j * TILE_SIZE);
			window.draw(bgTiles[TILE_REGULAR]);
		}
	}

	// Draw the borders
	window.draw(bgTiles[TILE_UPPER_LEFT]);
	for (unsigned short i = 1; i < btn_x; ++i) {
		bgTiles[TILE_UPPER].setPosition(i * TILE_SIZE, 0);
		window.draw(bgTiles[TILE_UPPER]);
	}
	for (unsigned short i = 1; i < btn_y; ++i) {
		bgTiles[TILE_LEFT].setPosition(0, i * TILE_SIZE);
		window.draw(bgTiles[TILE_LEFT]);
	}
	window.draw(bgTiles[TILE_UPPER_RIGHT]);

	for (unsigned short i = 1; i < (btn_y); ++i) {
		bgTiles[TILE_RIGHT].setPosition(btn_x * TILE_SIZE, i * TILE_SIZE);
		window.draw(bgTiles[TILE_RIGHT]);
	}

	window.draw(bgTiles[TILE_LOWER_RIGHT]);
	for (unsigned short i = 1; i < btn_x; ++i) {
		bgTiles[TILE_LOWER].setPosition(i * TILE_SIZE, btn_y * TILE_SIZE);
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
	bool player_set[] = { false, false };
	for (unsigned int i = 0; i < tilemap.length(); ++i) {
		EntityType et = Game::entityFromLetter(tilemap[i]);
		if (et == EntityType::UNKNOWN) return false;
		if (et == EntityType::PLAYER1) {
			if (player_set[0]) return false;
			player_set[0] = true;
		}
		if (et == EntityType::PLAYER2) {
			if (player_set[1]) return false;
			player_set[1] = true;
		}
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
