#include "GameCache.hpp"

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
