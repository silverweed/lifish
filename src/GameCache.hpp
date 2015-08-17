#pragma once

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace Game {

/**
 * Keeps the loaded textures and sounds in memory for faster loading;
 * works as an associative set name => pointer-to-resource 
 */
class GameCache final : private sf::NonCopyable {

	/** The game textures */
	std::map<std::string, sf::Texture*> textures;
	
	/** The sound buffers used by sounds */
	std::map<std::string, sf::SoundBuffer*> soundBuffers;

	/** The game fonts */
	std::map<std::string, sf::Font*> fonts;
	
	/** The queue of sound being played, which gets "garbage collected"
	 *  by the game cache each SOUNDS_GC_DELAY game cycles.
	 */
	std::vector<sf::Sound*> sounds;


	sf::Texture* _getTexture(const std::string& name) const;

	sf::SoundBuffer* _getSound(const std::string& name) const;

	sf::Font* _getFont(const std::string& name) const;
public:
	constexpr static int SOUNDS_GC_DELAY = 1024;

	GameCache();
	~GameCache();

	/** Tries to load `texture_name` into `texture`; if `texture_name` is already
	 *  in the cache, load it from there; else, load from file and put the
	 *  loaded image into the cache.
	 */
	bool loadTexture(sf::Texture& texture, const std::string& texture_name);

	/** Tries to load `sound_name` into `sound`; if `sound_name` is already
	 *  in the cache, load it from there; else, load from file and put the
	 *  loaded soundbuffer into the cache.
	 */
	bool loadSound(sf::Sound& sound, const std::string& sound_name);

	/** Requests that the sound `sound_name` be played. This means the following
	 *  actions:
	 *  1- if a sound buffer `sound_name` is not in cache, load if from file;
	 *  2- create a new sf::Sound and store it in the `sounds` queue;
	 *  3- play it.
	 */
	void playSound(const std::string& sound_name);

	/** Removes from `sounds` all the sounds which are not playing. */
	void gcSounds();

	bool loadFont(sf::Font& font, const std::string& font_name);
};

}
