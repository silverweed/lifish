#include "GameCache.hpp"
#include "core.hpp"
#include "Options.hpp"
#include <iostream>

using Game::GameCache;

GameCache::GameCache() {
	sounds.reserve(MAX_PARALLEL_SOUNDS);
}

sf::Texture* GameCache::loadTexture(const std::string& texture_name) {
	// Check if image is already in cache
	auto it = textures.find(texture_name);
	if (it != textures.end()) 
		return &it->second;

	// Not in cache: load from file
	auto& txt = textures[texture_name];
	if (!txt.loadFromFile(texture_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load texture " 
			<< texture_name << " from file!" << std::endl;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << texture_name << std::endl;
	}
#endif
	return &txt;
}

bool GameCache::loadSound(sf::Sound& sound, const std::string& sound_name) {
	// Check if sound buffer is already in cache
	auto it = soundBuffers.find(sound_name);
	if (it != soundBuffers.end()) {
		sound.setBuffer(it->second);
		return true;
	}
	// Load from file and update the cache
	auto& buf = soundBuffers[sound_name];
	if (!buf.loadFromFile(sound_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load sound " << sound_name << " from file!" << std::endl;
		return false;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << sound_name << std::endl;
	}
#endif
	sound.setBuffer(buf);
	return true;
}

void GameCache::playSound(const std::string& sound_name) {
	if (Game::options.soundsMute || sounds.size() == MAX_PARALLEL_SOUNDS) return;
	sounds.push_back(sf::Sound());
	auto& sound = sounds.back();
	if (!loadSound(sound, sound_name))
		return;

	sound.setVolume(Game::options.soundsVolume);
	sound.play();
}

void GameCache::gcSounds() {
	for (auto it = sounds.begin(); it != sounds.end(); ) {
		if (it->getStatus() != sf::Sound::Status::Playing)
			it = sounds.erase(it);
		else
			++it;
	}
}

sf::Font* GameCache::loadFont(const std::string& font_name) {
	auto it = fonts.find(font_name);
	if (it != fonts.end())
		return &it->second;

	// Load from file and update the cache
	auto& font = fonts[font_name];
	if (!font.loadFromFile(font_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load font " 
			<< font_name << " from file!" << std::endl;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << font_name << std::endl;
	}
#endif
	return &font;
}
