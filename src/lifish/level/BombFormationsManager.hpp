#pragma once

#include <SFML/System.hpp>
#include <vector>

namespace lif {

class BaseLevelManager;

using PatternSignature = float;

struct BombFormation {
	std::vector<sf::Vector2i> tiles;
	PatternSignature signature = 0;

	void addBomb(sf::Vector2i tile);
};

class BombFormationsManager {
	std::vector<BombFormation> formations;

public:
	void addBomb(const lif::BaseLevelManager& blm, sf::Vector2i start);

	void debugDisplay() const;
};

}
