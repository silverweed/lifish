#include "Level.hpp"
#include "Game.hpp"
#include <iostream>

using namespace Game;

Level::Level(const std::string& texture_name) {
	// Load background texture
	if (!bgTexture.loadFromFile(texture_name)) 
		std::cerr << "[Level.cpp] Error: couldn't load texture " << texture_name << std::endl;

	for (unsigned short i = 0; i < bgTiles.size(); ++i)
		bgTiles[i].setTexture(bgTexture);

	bgTiles[TILE_REGULAR].setTextureRect(sf::IntRect(3*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_LEFT].setPosition(sf::Vector2f(0, 0));
	bgTiles[TILE_UPPER].setTextureRect(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_UPPER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH-1), 0));
	bgTiles[TILE_RIGHT].setTextureRect(sf::IntRect(3*TILE_SIZE, 0, -TILE_SIZE, TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setTextureRect(sf::IntRect(TILE_SIZE, TILE_SIZE, -TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH-1), TILE_SIZE * (LEVEL_HEIGHT-1)));
	bgTiles[TILE_LOWER].setTextureRect(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_LEFT].setTextureRect(sf::IntRect(0, TILE_SIZE, TILE_SIZE, -TILE_SIZE));
	bgTiles[TILE_LOWER_LEFT].setPosition(sf::Vector2f(0, TILE_SIZE * (LEVEL_HEIGHT-1)));
	bgTiles[TILE_LEFT].setTextureRect(sf::IntRect(2*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}

Level::~Level() {}

void Level::draw(sf::RenderTarget& window) {
	window.draw(bgTiles[TILE_UPPER_LEFT]);
	for (unsigned short i = 1; i < LEVEL_WIDTH - 1; ++i) {
		bgTiles[TILE_UPPER].setPosition(sf::Vector2f(i * TILE_SIZE, 0));
		window.draw(bgTiles[TILE_UPPER]);
	}
	window.draw(bgTiles[TILE_UPPER_RIGHT]);
	for (unsigned short i = 1; i < LEVEL_HEIGHT - 1; ++i) {
		bgTiles[TILE_RIGHT].setPosition(sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH-1), i * TILE_SIZE));
		window.draw(bgTiles[TILE_RIGHT]);
	}
	window.draw(bgTiles[TILE_LOWER_RIGHT]);
	for (unsigned short i = 1; i < LEVEL_WIDTH - 1; ++i) {
		bgTiles[TILE_LOWER].setPosition(sf::Vector2f(i * TILE_SIZE, TILE_SIZE * (LEVEL_HEIGHT-1)));
		window.draw(bgTiles[TILE_LOWER]);
	}
	window.draw(bgTiles[TILE_LOWER_LEFT]);
	for (unsigned short i = 1; i < LEVEL_HEIGHT - 1; ++i) {
		bgTiles[TILE_LEFT].setPosition(sf::Vector2f(0, i * TILE_SIZE));
		window.draw(bgTiles[TILE_LEFT]);
	}
	for (unsigned short i = 1; i < LEVEL_WIDTH - 1; ++i) {
		for (unsigned short j = 1; j < LEVEL_HEIGHT - 1; ++j) {
			bgTiles[TILE_REGULAR].setPosition(sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE));
			window.draw(bgTiles[TILE_REGULAR]);
		}
	}
}
