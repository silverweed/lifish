#pragma once

#include <memory>
#include <SFML/Audio/InputSoundFile.hpp>
#include "Component.hpp"
#include "Track.hpp"

class LoopingMusic;

namespace Game {

class Music : public Game::Component {
	/** The input sound file for the BGM */
	sf::InputSoundFile musicInput;

	/** The music for this level */
	std::shared_ptr<LoopingMusic> music;

	/** The music track data */
	Game::Track track;

public:
	explicit Music(Game::Entity& owner, const Game::Track& track);
	
	std::shared_ptr<LoopingMusic> getMusic() const { return music; }
	const Game::Track& getTrack() const { return track; }
};

}
