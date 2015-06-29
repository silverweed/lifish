#include "GameCache.hpp"
#include <iostream>

using Game::GameCache;

GameCache::GameCache() {}

GameCache::~GameCache() {
	for (auto& it : textures) {
		if (it.second != nullptr)
			delete it.second;
	}
}

sf::Image* GameCache::getTexture(const std::string& name) {
	auto it = textures.find(name);
	if (it == textures.end())
		return nullptr;
	return it->second;
}

bool GameCache::loadTexture(sf::Texture& texture, const std::string& texture_name) {
	// Check if image is already in cache
	sf::Image *img = getTexture(texture_name);
	bool loaded = false;
	if (img != nullptr) {
		if (!texture.loadFromImage(*img)) 
			std::cerr << "[GameCache.cpp] Error: couldn't load texture " << texture_name << " from memory!" << std::endl;
		else {
			loaded = true;
		}
	}
	// Load from file and update the cache
	if (!loaded) {
		sf::Image *img = new sf::Image;
		if (!img->loadFromFile(texture_name))
			std::cerr << "[GameCache.cpp] Error: couldn't load texture " << texture_name << " from file!" << std::endl;
		else if (texture.loadFromImage(*img)) {
			loaded = true;
			//texture.setSmooth(true);
			putTexture(texture_name, img);
		} else 
			std::cerr << "[GameCache.cpp] Error: couldn't load texture " << texture_name << " from image!" << std::endl;
	}
	return loaded;
}
