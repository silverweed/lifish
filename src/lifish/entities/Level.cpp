#include "Level.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "LevelSet.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>

using Game::Level;
using Game::pwd;
using Game::DIRSEP;
using Game::LEVEL_WIDTH;
using Game::LEVEL_HEIGHT;
using Game::TILE_SIZE;
using Game::EntityType;

Level::Level(const Game::LevelSet& _levelSet) 
	: Game::Entity()
	, levelSet(_levelSet) 
{}

Game::Entity* Level::init() {
	if (initialized) return this;
	
	if (!_setTilemap(levelInfo.tilemap))
		return nullptr;

	addComponent(new Game::Music(*this, levelInfo.track));
	_loadTextures();

	levelnumtext = addComponent(new Game::LevelNumText(*this, levelInfo.levelnum));

	initialized = true;

	return this;
}

void Level::_loadTextures() {
	std::stringstream ss;
	ss << "bg" << levelInfo.tileIDs.bg << ".png";
	// Load background texture
	bgTexture = Game::cache.loadTexture(Game::getAsset("graphics", ss.str()));
	bgTexture->setSmooth(true);
	bgTexture->setRepeated(true);
	// Load borderTexture
	borderTexture = Game::cache.loadTexture(Game::getAsset("graphics", "border.png"));
	borderTexture->setSmooth(true);
	_loadTiles();
}

void Level::_loadTiles() {
	// Background
	bgTiles[TILE_REGULAR].setTexture(*bgTexture);
	bgTiles[TILE_REGULAR].setTextureRect(sf::IntRect(0, 0, TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT));

	const unsigned short b = (levelInfo.tileIDs.border-1) * TILE_SIZE;

	// Create images with texture subdivisions for border
	sf::Image image(borderTexture->copyToImage());
	
	bgTextures[TILE_UPPER].loadFromImage(image, sf::IntRect(0, b, TILE_SIZE, TILE_SIZE));
	bgTextures[TILE_UPPER].setRepeated(true);
	bgTiles[TILE_UPPER].setTexture(bgTextures[TILE_UPPER]);
	bgTiles[TILE_UPPER].setTextureRect(sf::IntRect(0, 0, TILE_SIZE * LEVEL_WIDTH, TILE_SIZE));

	// TILE_UPPER y-mirrored
	bgTextures[TILE_LOWER].loadFromImage(image, sf::IntRect(0, b, TILE_SIZE, TILE_SIZE));
	bgTextures[TILE_LOWER].setRepeated(true);
	bgTiles[TILE_LOWER].setTexture(bgTextures[TILE_LOWER]);
	bgTiles[TILE_LOWER].setTextureRect(sf::IntRect(0, TILE_SIZE, TILE_SIZE * LEVEL_WIDTH, -TILE_SIZE));

	bgTextures[TILE_LEFT].loadFromImage(image, sf::IntRect(TILE_SIZE, b, TILE_SIZE, TILE_SIZE));
	bgTextures[TILE_LEFT].setRepeated(true);
	bgTiles[TILE_LEFT].setTexture(bgTextures[TILE_LEFT]);
	bgTiles[TILE_LEFT].setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE * LEVEL_HEIGHT));

	// TILE_LEFT x-mirrored
	bgTextures[TILE_RIGHT].loadFromImage(image, sf::IntRect(TILE_SIZE, b, TILE_SIZE, TILE_SIZE));
	bgTextures[TILE_RIGHT].setRepeated(true);
	bgTiles[TILE_RIGHT].setTexture(bgTextures[TILE_RIGHT]);
	bgTiles[TILE_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE * LEVEL_HEIGHT));

	bgTiles[TILE_UPPER_LEFT].setTexture(*borderTexture);
	bgTiles[TILE_UPPER_LEFT].setTextureRect(sf::IntRect(2 * TILE_SIZE, b, TILE_SIZE, TILE_SIZE));

	// TILE_UPPER_LEFT x-mirrored
	bgTiles[TILE_UPPER_RIGHT].setTexture(*borderTexture);
	bgTiles[TILE_UPPER_RIGHT].setTextureRect(sf::IntRect(2 * TILE_SIZE + TILE_SIZE, b, -TILE_SIZE, TILE_SIZE));

	// TILE_UPPER_LEFT x-mirrored and y-mirrored
	bgTiles[TILE_LOWER_RIGHT].setTexture(*borderTexture);
	bgTiles[TILE_LOWER_RIGHT].setTextureRect(sf::IntRect(
				2 * TILE_SIZE + TILE_SIZE, 
				b + TILE_SIZE, -TILE_SIZE, -TILE_SIZE));

	// TILE_UPPER_LEFT y-mirrored
	bgTiles[TILE_LOWER_LEFT].setTexture(*borderTexture);
	bgTiles[TILE_LOWER_LEFT].setTextureRect(sf::IntRect(2 * TILE_SIZE, b + TILE_SIZE, TILE_SIZE, -TILE_SIZE));

	// Set angles' position once and for all
	bgTiles[TILE_REGULAR].setPosition(TILE_SIZE, TILE_SIZE);
	bgTiles[TILE_UPPER].setPosition(TILE_SIZE, 0);
	bgTiles[TILE_LOWER].setPosition(TILE_SIZE, TILE_SIZE * (LEVEL_HEIGHT+1));
	bgTiles[TILE_LEFT].setPosition(0, TILE_SIZE);
	bgTiles[TILE_RIGHT].setPosition(TILE_SIZE * (LEVEL_WIDTH+1), TILE_SIZE);
	bgTiles[TILE_UPPER_LEFT].setPosition(0, 0);
	bgTiles[TILE_UPPER_RIGHT].setPosition(TILE_SIZE * (LEVEL_WIDTH+1), 0);
	bgTiles[TILE_LOWER_RIGHT].setPosition(TILE_SIZE * (LEVEL_WIDTH+1), TILE_SIZE * (LEVEL_HEIGHT+1));
	bgTiles[TILE_LOWER_LEFT].setPosition(0, TILE_SIZE * (LEVEL_HEIGHT+1));
}

void Level::setOrigin(const sf::Vector2f& offset) {
	for (unsigned short i = 0; i < bgTiles.size(); ++i)
		bgTiles[i].setOrigin(offset);
	levelnumtext->setOrigin(offset);
}

void Level::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	// Draw the level background
	window.draw(bgTiles[TILE_REGULAR], states);

	// Draw the borders
	window.draw(bgTiles[TILE_UPPER_LEFT], states);
	window.draw(bgTiles[TILE_UPPER], states);
	window.draw(bgTiles[TILE_LEFT], states);
	window.draw(bgTiles[TILE_UPPER_RIGHT], states);
	window.draw(bgTiles[TILE_RIGHT], states);
	window.draw(bgTiles[TILE_LOWER_RIGHT], states);
	window.draw(bgTiles[TILE_LOWER], states);
	window.draw(bgTiles[TILE_LOWER_LEFT], states);

	// Draw the level number
	if (levelnumtext != nullptr)
		window.draw(*levelnumtext, states);
}

EntityType Level::getTile(unsigned short left, unsigned short top) const {
	if (left >= LEVEL_WIDTH || top >= LEVEL_HEIGHT) 
		return EntityType::UNKNOWN;
	return tiles[top][left];
}

bool Level::_setTilemap(const std::string& tilemap) {
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

std::string Level::getTilemap() const {
	static bool called = false;
	static std::stringstream ss;

	if (called)
		return ss.str();

	for (unsigned short i = 0; i < LEVEL_HEIGHT; ++i) {
		for (unsigned short j = 0; j < LEVEL_WIDTH; ++j) {
			ss << tiles[i][j] << " ";
		}
		ss << std::endl;
	}

	called = true;
	return ss.str();
}

std::string Level::toString() const {
	static bool called = false;
	static std::stringstream ss;

	if (called)
		return ss.str();

	ss << "Level Info:\r\n" 
	   << "-----------\r\n"
	   << "Level " << levelInfo.levelnum << "\r\n"
	   << "Time: " << levelInfo.time << " s\r\n"
	   << "Tiles: {\r\n"
	   << "    bg: " << levelInfo.tileIDs.bg << "\r\n"
	   << "    border: " << levelInfo.tileIDs.border << "\r\n"
	   << "    fixed: " << levelInfo.tileIDs.fixed << "\r\n"
	   << "    breakable: " << levelInfo.tileIDs.breakable << "\r\n}\r\n"
	   << "Music: " << get<Game::Music>()->getTrack().name << "\r\n"
	   << "Belongs to: >>>\r\n"
	   << levelSet.toString()
	   << "<<<\r\n"
	   << "Tilemap:\r\n"
	   << getTilemap();

	called = true;
	return ss.str();
}
