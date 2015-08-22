#include "GameCache.hpp"
#include <iostream>

using Game::GameCache;

GameCache::GameCache() {
	sounds.reserve(1024);
}

GameCache::~GameCache() {
	for (auto& it : textures)
		if (it.second != nullptr)
			delete it.second;

	for (auto& it : soundBuffers)
		if (it.second != nullptr)
			delete it.second;

	for (auto& it : fonts)
		if (it.second != nullptr)
			delete it.second;

	for (auto& sound : sounds)
		delete sound;
}

sf::Texture* GameCache::_getTexture(const std::string& name) const {
	auto it = textures.find(name);
	if (it == textures.end())
		return nullptr;
	return it->second;
}

bool GameCache::loadTexture(sf::Texture& texture, const std::string& texture_name) {
	// Check if image is already in cache
	auto txt = _getTexture(texture_name);
	if (txt != nullptr) {
		texture = *txt;
		return true;
	}
	// Not in cache: load from file
	txt = new sf::Texture;
	if (!txt->loadFromFile(texture_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load texture " << texture_name << " from file!" << std::endl;
		return false;
	}
	texture = *txt;
	textures[texture_name] = txt;
	return true;
}

sf::SoundBuffer* GameCache::_getSound(const std::string& name) const {
	auto it = soundBuffers.find(name);
	if (it == soundBuffers.end())
		return nullptr;
	return it->second;
}

bool GameCache::loadSound(sf::Sound& sound, const std::string& sound_name) {
	// Check if sound buffer is already in cache
	auto buf = _getSound(sound_name);
	if (buf != nullptr) {
		sound.setBuffer(*buf);
		return true;
	}
	// Load from file and update the cache
	buf = new sf::SoundBuffer;
	if (!buf->loadFromFile(sound_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load sound " << sound_name << " from file!" << std::endl;
		return false;
	}
	sound.setBuffer(*buf);
	soundBuffers[sound_name] = buf;
	return true;
}

void GameCache::playSound(const std::string& sound_name) {
	auto sound = new sf::Sound;
	if (!loadSound(*sound, sound_name)) {
		delete sound;
		return;
	}
	sounds.push_back(sound);
	sound->play();
}

void GameCache::gcSounds() {
	for (auto it = sounds.begin(); it != sounds.end(); ) {
		if ((*it)->getStatus() != sf::Sound::Status::Playing) {
			delete *it;
			it = sounds.erase(it);
		} else {
			++it;
		}
	}
}

sf::Font* GameCache::_getFont(const std::string& name) const {
	auto it = fonts.find(name);
	if (it == fonts.end())
		return nullptr;
	return it->second;
}

sf::Font* GameCache::loadFont(const std::string& font_name) {
	auto buf = _getFont(font_name);
	if (buf != nullptr) {
		return buf;
	}
	// Load from file and update the cache
	buf = new sf::Font;
	if (!buf->loadFromFile(font_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load font " << font_name << " from file!" << std::endl;
		return nullptr;
	}
	return fonts[font_name] = buf;
}
