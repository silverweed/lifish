#pragma once

#include "sid.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace lif {

/**
 * Keeps the loaded textures and sounds in memory for faster loading;
 * works as an associative set name => pointer-to-resource
 */
class GameCache final : private sf::NonCopyable {
	std::size_t maxParallelSounds = 10;

	/** The game textures */
	std::unordered_map<lif::StringId, sf::Texture> textures;

	/** The sound buffers used by sounds */
	std::unordered_map<lif::StringId, sf::SoundBuffer> soundBuffers;

	/** The game fonts */
	std::unordered_map<lif::StringId, sf::Font> fonts;

	/** The queue of sound being played. Ended sounds are removed from this
	 *  queue as needed when `playSound()` is called. At most MAX_PARALLEL_SOUNDS
	 *  can be playing at once.
	 */
	std::list<sf::Sound> sounds;

public:
	explicit GameCache();

	void setMaxParallelSounds(std::size_t n);

	/** If the texture loaded from `texture_name` already exists in the cache,
	 *  return its pointer; else try to load it from `texture_name` and return either
	 *  a pointer to it, or nullptr if the loading failed.
	 */
	sf::Texture* loadTexture(const std::string& textureName);

	/** Tries to load `sound_name` into `sound`; if `sound_name` is already
	 *  in the cache, load it from there; else, load from file and put the
	 *  loaded soundbuffer into the cache.
	 */
	bool loadSound(sf::Sound& sound, const std::string& soundName);

	/** Requests that the sound `sound_name` be played. This means the following
	 *  actions:
	 *  1- if lif::options.soundsMute is true, do nothing and return;
	 *  2- if `sounds.size() >= MAX_PARALLEL_SOUNDS`, check if there is a sound which
	 *     is not playing anymore; remove it if found, else return;
	 *  3- if a sound buffer `sound_name` is not in cache, load if from file;
	 *  4- create a new sf::Sound and store it in the `sounds` queue;
	 *  5- play it.
	 */
	void playSound(const std::string& soundName);

	/** Loads the font `font_name` (either from the cache or from file)
	 *  and returns a pointer to it.
	 */
	sf::Font* loadFont(const std::string& fontName);

	/** If the GameCache is a global object, this method must be called
	 *  before exiting the program to prevent crashes due to improper
	 *  automatic cleanup.
	 */
	void finalize();
};

}
