#pragma once
/**
 * Keeps the loaded textures in memory for faster loading;
 * works as an associative set texture_name => image
 */

#include <map>
#include <string>
#include <SFML/Graphics.hpp>

namespace Game {

class GameCache {
	std::map<std::string, sf::Image*> textures;
public:
	GameCache();
	~GameCache();

	sf::Image* getTexture(const std::string& name);
	void putTexture(const std::string& name, sf::Image* img) { textures[name] = img; }
};

}
