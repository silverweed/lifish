#include "MusicManager.hpp"

using Game::MusicManager;

void MusicManager::play() {
	if (music != nullptr) {
		music->play();
	}
}

void MusicManager::stop() {
	if (music != nullptr)
		music->stop();
}

void MusicManager::setVolume(float volume) {
	if (music != nullptr)
		music->setVolume(volume);
}
