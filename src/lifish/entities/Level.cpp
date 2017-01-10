#include "Level.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "LevelSet.hpp"
#include "utils.hpp"
#include "LoopingMusic.hpp"
#include "Drawable.hpp"
#include "ShadedText.hpp"
#include "Music.hpp"
#include "LevelNumText.hpp"
#include <sstream>

using lif::Level;
using lif::pwd;
using lif::DIRSEP;
using lif::LEVEL_WIDTH;
using lif::LEVEL_HEIGHT;
using lif::TILE_SIZE;
using lif::EntityType;

Level::Level(const lif::LevelSet& _levelSet) 
	: lif::Entity()
	, levelSet(_levelSet) 
{}

lif::Entity* Level::init() {
	if (initialized) return this;
	
	if (!_setTilemap(levelInfo.tilemap))
		return nullptr;

	addComponent(new lif::Music(*this, levelInfo.track));
	_loadTextures();

	levelnumtext = addComponent(new lif::LevelNumText(*this, levelInfo.levelnum));

	initialized = true;

	return this;
}

void Level::_loadTextures() {
	std::stringstream ss;
	ss << "bg" << levelInfo.tileIDs.bg << ".png";
	// Load background texture
	bgTexture = lif::cache.loadTexture(lif::getAsset("graphics", ss.str()));
	bgTexture->setSmooth(true);
	bgTexture->setRepeated(true);
	// Load borderTexture
	ss.str("");
	ss << "border" << levelInfo.tileIDs.border << ".png";
	borderTexture = lif::cache.loadTexture(lif::getAsset("graphics", ss.str()));
	borderTexture->setSmooth(true);
	_loadTiles();
}

void Level::_loadTiles() {
	// Background
	bgSprite.setTexture(*bgTexture);
	bgSprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT));
	bgSprite.setPosition(TILE_SIZE, TILE_SIZE);

	// Border
	borderSprite.setTexture(*borderTexture);
	borderSprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE * (LEVEL_WIDTH + 2), TILE_SIZE * (LEVEL_HEIGHT + 2)));
}

void Level::setOrigin(const sf::Vector2f& offset) {
	lif::Entity::setOrigin(offset);
	bgSprite.setOrigin(offset);
	borderSprite.setOrigin(offset);
	levelnumtext->setOrigin(offset);
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
		EntityType et = lif::entityFromLetter(tilemap[i]);
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

	for (unsigned i = 0; i < LEVEL_HEIGHT; ++i) {
		for (unsigned j = 0; j < LEVEL_WIDTH; ++j) {
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
	   << "Music: " << get<lif::Music>()->getTrack().name << "\r\n"
	   << "Belongs to: >>>\r\n"
	   << levelSet.toString()
	   << "<<<\r\n"
	   << "Tilemap:\r\n"
	   << getTilemap();

	called = true;
	return ss.str();
} 

const sf::Drawable* Level::getNumText() const {
	return levelnumtext;
}
