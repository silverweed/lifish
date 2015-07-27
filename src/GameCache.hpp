#pragma once

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace Game {

/**
 * Keeps the loaded textures in memory for faster loading;
 * works as an associative set texture_name => image
 */
class GameCache : private sf::NonCopyable {
	std::map<std::string, sf::Image*> textures;
public:
	GameCache();
	~GameCache();

	sf::Image* getTexture(const std::string& name);
	void putTexture(const std::string& name, sf::Image* img) { textures[name] = img; }

	/** Tries to load `texture_name` into `texture`; if `texture_name` is already
	 *  in the cache, load it from there; else, load from file and put the
	 *  loaded image into the cache.
	 */
	bool loadTexture(sf::Texture& texture, const std::string& texture_name);
};

}
