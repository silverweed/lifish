#include "Music.hpp"
#include <iostream>

using Game::Music;

Music::Music(Game::Entity *const owner, const Game::Track& track)
	: track(track)
	, Game::Component(owner)
{
	if (!musicInput.openFromFile(track.name)) {
		std::cerr << "[Level.cpp] Error: couldn't load music " << track.name << " from file!" << std::endl;
	} else {
		music = new LoopingMusic(musicInput);
		music->setLoopPoints(sf::seconds(track.loopstart), sf::seconds(track.loopend));
		music->setLoop(true);
	}
}
