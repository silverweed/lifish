#include "Music.hpp"
#include "LoopingMusic.hpp"
#include <iostream>

using lif::Music;

Music::Music(lif::Entity& owner, const lif::Track& track)
	: lif::Component(owner)
	, track(track)
{
	if (!musicInput.openFromFile(track.name)) {
		std::cerr << "Error: couldn't load music " << track.name << " from file!" << std::endl;
	} else {
#ifndef RELEASE
		std::cerr << "[Music] Loaded " + track.name << std::endl;
		std::cerr << "loop: " << track.loopstart << " , " << track.loopend << std::endl;
#endif
		music = std::unique_ptr<LoopingMusic>(new LoopingMusic(musicInput));
		music->setLoopPoints(sf::seconds(track.loopstart), sf::seconds(track.loopend));
		music->setLoop(true);
	}
}
