#include "MusicManager.hpp"
#include "LoopingMusic.hpp"

#define DEF_PROXY(action) \
	MusicManager& MusicManager::  action  () { \
		if (!music.expired() && music.lock() != nullptr) { \
			music.lock()->  action  (); \
		} \
		return *this; \
	}

using lif::MusicManager;

MusicManager& MusicManager::set(std::shared_ptr<LoopingMusic> m) {
	stop();
	music = m;
	return *this;
}

DEF_PROXY(play)
DEF_PROXY(stop)
DEF_PROXY(pause)

MusicManager& MusicManager::setVolume(float volume) {
	if (!music.expired() && music.lock() != nullptr)
		music.lock()->setVolume(volume);
	return *this;
}
