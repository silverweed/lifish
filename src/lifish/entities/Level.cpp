#include "Level.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "LevelNumText.hpp"
#include "LevelSet.hpp"
#include "LoopingMusic.hpp"
#include "Music.hpp"
#include "ShadedText.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <sstream>
#include <iostream>

using lif::Level;
using lif::pwd;
using lif::DIRSEP;
using lif::TILE_SIZE;
using lif::EntityType;

Level::Level(const lif::LevelSet& _levelSet)
	: lif::Entity()
	, levelSet(_levelSet)
{}

lif::Entity* Level::init() {
	if (initialized) return this;

	if (!_setTilemap(info.tilemap))
		return nullptr;

	addComponent<lif::Music>(*this, info.track);
	_loadTextures();

	levelnumtext = addComponent<lif::LevelNumText>(*this, info.levelnum);

	initialized = true;

	return this;
}

void Level::_loadTextures() {
	std::stringstream ss;
	ss << "bg" << info.tileIDs.bg << ".png";
	// Load background texture
	bgTexture = lif::cache.loadTexture(lif::getAsset("graphics", ss.str()));
	bgTexture->setSmooth(true);
	bgTexture->setRepeated(true);
	// Load borderTexture
	ss.str("");
	ss << "border" << info.tileIDs.border << ".png";
	borderTexture = lif::cache.loadTexture(lif::getAsset("graphics", ss.str()));
	borderTexture->setSmooth(true);
	_loadTiles();
}

void Level::_loadTiles() {
	// Background
	bgSprite.setTexture(*bgTexture);
	bgSprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE * (2 + info.width), TILE_SIZE * (2 + info.height)));
	bgSprite.setPosition(0, 0);

	// Border
	borderSprite.setTexture(*borderTexture);
	borderSprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE * (info.width + 2), TILE_SIZE * (info.height + 2)));
}

void Level::setOrigin(const sf::Vector2f& offset) {
	lif::Entity::setOrigin(offset);
	bgSprite.setOrigin(offset);
	borderSprite.setOrigin(offset);
	levelnumtext->setOrigin(offset);
}

EntityType Level::getTile(int left, int top) const {
	if (left < 0 || top < 0 || left >= info.width || top >= info.height)
		return EntityType::UNKNOWN;
	return tiles[top * info.width + left];
}

bool Level::_setTilemap(const std::string& tilemap) {
	bool player_set[] = { false, false };
	tiles.reserve(info.width * info.height);

	for (unsigned i = 0; i < tilemap.length() && i < static_cast<unsigned>(info.width * info.height); ++i) {
		const auto et = lif::entityFromLetter(tilemap[i]);
		switch (et) {
		case EntityType::UNKNOWN:
			std::cerr << "Unknown entity type `" << et << "` at tile " << i << std::endl;
			return false;
		case EntityType::PLAYER1:
			if (player_set[0])
				return false;
			player_set[0] = true;
			break;
		case EntityType::PLAYER2:
			if (player_set[1])
				return false;
			player_set[1] = true;
			break;
		default:
			break;
		}
		tiles.emplace_back(et);
	}
	return true;
}

std::string Level::getTilemap() const {
	std::stringstream ss;

	for (int i = 0; i < info.height; ++i) {
		for (int j = 0; j < info.width; ++j) {
			ss << tiles[i * info.width + j] << " ";
		}
		ss << "\r\n";
	}

	return ss.str();
}

std::string Level::getTilemapRaw() const {
	std::stringstream ss;

	for (unsigned i = 0; i < tiles.size(); ++i)
		ss << tiles[i];

	return ss.str();
}

std::string Level::toString() const {
	static bool called = false;
	static std::stringstream ss;

	if (!called) {
		ss << "Level Info:\r\n"
		   << "-----------\r\n"
		   << "Level " << info.levelnum << "\r\n"
		   << "Time: " << info.time << " s\r\n"
		   << "Tiles: {\r\n"
		   << "    bg: " << info.tileIDs.bg << "\r\n"
		   << "    border: " << info.tileIDs.border << "\r\n"
		   << "    fixed: " << info.tileIDs.fixed << "\r\n"
		   << "    breakable: " << info.tileIDs.breakable << "\r\n}\r\n"
		   << "Music: " << get<lif::Music>()->getTrack().name << "\r\n"
		   << "Belongs to: >>>\r\n"
		   << levelSet.toString()
		   << "<<<\r\n"
		   << "Tilemap:\r\n"
		   << getTilemap();
		called = true;
	}

	return ss.str();
}
