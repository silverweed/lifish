#include "Music.hpp"
#include <iostream>

using Game::Music;

Music::Music(Game::Entity *const owner, const Game::Track& track)
	: track(track)
	, Game::Component(owner)
{
	if (!musicInput.openFromFile(track.name)) {
		std::cerr << "Error: couldn't load music " << track.name << " from file!" << std::endl;
	} else {
#ifndef RELEASE
		std::cerr << "[Music] Loaded " + track.name << std::endl;
#endif
		std::cerr << "loop: " << track.loopstart << " , " << track.loopend << std::endl;
		music = std::unique_ptr<LoopingMusic>(new LoopingMusic(musicInput));
		music->setLoopPoints(sf::seconds(track.loopstart), sf::seconds(track.loopend));
		music->setLoop(true);
	}
}
