#include "MusicManager.hpp"
#include "LoopingMusic.hpp"

using Game::MusicManager;

MusicManager& MusicManager::set(std::shared_ptr<LoopingMusic> m) {
	stop();
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

MusicManager& MusicManager::pause() {
	if (music != nullptr)
		music->pause();
	return *this;
}

MusicManager& MusicManager::setVolume(float volume) {
	if (music != nullptr)
		music->setVolume(volume);
	return *this;
}
