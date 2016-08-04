#include "MusicManager.hpp"

using Game::MusicManager;

MusicManager& MusicManager::set(LoopingMusic *m) {
	music = m;
	return *this;
}

MusicManager& MusicManager::play() {
	if (music != nullptr) {
		music->play();
	}
	return *this;
}

MusicManager& MusicManager::stop() {
	if (music != nullptr)
		music->stop();
	return *this;
}

MusicManager& MusicManager::setVolume(float volume) {
	if (music != nullptr)
		music->setVolume(volume);
	return *this;
}
