#include "GameCache.hpp"
#include "Options.hpp"
#include "core.hpp"
#include <iostream>

using lif::GameCache;

GameCache::GameCache() {}

void GameCache::setMaxParallelSounds(std::size_t n) {
	maxParallelSounds = n;
}

sf::Texture* GameCache::loadTexture(const std::string& textureName) {
	// Check if image is already in cache
	const auto nameSid = lif::sid(textureName);
	auto it = textures.find(nameSid);
	if (it != textures.end())
		return &it->second;

	// Not in cache: load from file
	auto& txt = textures[nameSid];
	if (!txt.loadFromFile(textureName)) {
		std::cerr << "[GameCache] Error: couldn't load texture " << textureName << " from file!\r\n";
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
	const auto nameSid = lif::sid(soundName);
	auto it = soundBuffers.find(nameSid);
	if (it != soundBuffers.end()) {
		sound.setBuffer(it->second);
		return true;
	}
	// Load from file and update the cache
	auto& buf = soundBuffers[nameSid];
	if (!buf.loadFromFile(soundName)) {
		std::cerr << "[GameCache] Error: couldn't load sound " << soundName << " from file!\r\n";
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

	// Find a free slot to put this sound into, or discard oldest sound
	auto it = sounds.begin();
	unsigned idx = 0;
	while (it != sounds.end() && idx < maxParallelSounds) {
		if (it->getStatus() != sf::Sound::Status::Playing) {
			sounds.erase(it);
			break;
		}
		++it, ++idx;
	}

	// no room
	if (idx == maxParallelSounds) {
		sounds.pop_front();
	}
	sounds.push_back(sf::Sound());
	auto& sound = sounds.back();
	if (!loadSound(sound, soundName))
		return;

	sound.setVolume(lif::options.soundsVolume);
	sound.play();
}

sf::Font* GameCache::loadFont(const std::string& fontName) {
	const auto nameSid = lif::sid(fontName);
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
