#include "GameCache.hpp"
#include "core.hpp"
#include "Options.hpp"
#include <iostream>

using lif::GameCache;

GameCache::GameCache() {
	sounds.reserve(maxParallelSounds);
}

void GameCache::setMaxParallelSounds(std::size_t n) {
	maxParallelSounds = n;
	sounds.reserve(maxParallelSounds);
}

sf::Texture* GameCache::loadTexture(const std::string& textureName) {
	// Check if image is already in cache
	const auto nameSid = sid(textureName);
	auto it = textures.find(nameSid);
	if (it != textures.end())
		return &it->second;

	// Not in cache: load from file
	auto& txt = textures[nameSid];
	if (!txt.loadFromFile(textureName)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load texture " << textureName << " from file!\r\n";
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << textureName << std::endl;
	}
#endif
	return &txt;
}

bool GameCache::loadSound(sf::Sound& sound, const std::string& soundName) {
	// Check if sound buffer is already in cache
	const auto nameSid = sid(soundName);
	auto it = soundBuffers.find(nameSid);
	if (it != soundBuffers.end()) {
		sound.setBuffer(it->second);
		return true;
	}
	// Load from file and update the cache
	auto& buf = soundBuffers[nameSid];
	if (!buf.loadFromFile(soundName)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load sound " << soundName << " from file!\r\n";
		return false;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << soundName << std::endl;
	}
#endif
	sound.setBuffer(buf);
	return true;
}

void GameCache::playSound(const std::string& soundName) {
	if (lif::options.soundsMute) return;

	unsigned idx = 0;
	do {
		if (idx == sounds.size()) {
			// Sounds queue has less than `maxParallelSounds` sounds
			sounds.emplace_back(sf::Sound());
			break;
		}
		if (sounds[idx].getStatus() != sf::Sound::Status::Playing)
			break;
		++idx;
	} while (idx < maxParallelSounds);

	// No empty slot left
	if (idx == maxParallelSounds) return;

	// Replace this dead sound with the new one
	auto& sound = sounds[idx];
	if (!loadSound(sound, soundName))
		return;

	sound.setVolume(lif::options.soundsVolume);
	sound.play();
}

sf::Font* GameCache::loadFont(const std::string& fontName) {
	const auto nameSid = sid(fontName);
	auto it = fonts.find(nameSid);
	if (it != fonts.end())
		return &it->second;

	// Load from file and update the cache
	auto& font = fonts[nameSid];
	if (!font.loadFromFile(fontName)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load font " << fontName << " from file!\r\n";
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << fontName << std::endl;
	}
#endif
	return &font;
}

void GameCache::finalize() {
	textures.clear();
	sounds.clear();
	soundBuffers.clear();
	fonts.clear();
}
