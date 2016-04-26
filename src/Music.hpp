#pragma once

#include "Component.hpp"
#include "Track.hpp"
#include "LoopingMusic.hpp"

namespace Game {

class Music : public Game::Component {
	/** The input sound file for the BGM */
	sf::InputSoundFile musicInput;

	/** The music for this level */
	LoopingMusic *music = nullptr;

	/** The music track data */
	Game::Track track;

public:
	explicit Music(Game::Entity *const owner, const Game::Track& track);
	
	LoopingMusic* getMusic() const { return music; }
	const Game::Track& getTrack() const { return track; }
};

}
