#pragma once

#include <memory>
#include <SFML/Audio/InputSoundFile.hpp>
#include "Component.hpp"
#include "Track.hpp"

class LoopingMusic;

namespace lif {

class Music : public lif::Component {
	/** The input sound file for the BGM */
	sf::InputSoundFile musicInput;

	/** The music for this level */
	std::shared_ptr<LoopingMusic> music;

	/** The music track data */
	lif::Track track;

public:
	explicit Music(lif::Entity& owner, const lif::Track& track);

	std::shared_ptr<LoopingMusic> getMusic() const { return music; }
	const lif::Track& getTrack() const { return track; }
};

}
